# Timeout Handling in Web Servers - Best Practices

## Why Timeouts Are Important

Without proper timeouts, your server is vulnerable to:
1. **Resource Exhaustion**: Slow clients can hold connections open indefinitely
2. **DoS Attacks**: Attackers can open many slow connections (Slowloris attack)
3. **Memory Leaks**: Long-running requests consume resources
4. **Zombie Connections**: Dead clients that never close properly

---

## Types of Timeouts You Should Handle

### 1. **Client Connection Timeout** (Read/Write Inactive)
**What**: Client connected but no data received/sent for X seconds
**When**: Between any read/write operations
**Typical Value**: 30-60 seconds
**Purpose**: Close idle connections

### 2. **Request Header Timeout**
**What**: Time to receive complete HTTP headers
**When**: From connection until headers are fully received
**Typical Value**: 10-20 seconds
**Purpose**: Prevent slow header attacks (Slowloris)

### 3. **Request Body Timeout**
**What**: Time to receive complete request body
**When**: While reading POST/PUT data
**Typical Value**: 60-300 seconds (depends on max upload size)
**Purpose**: Prevent slow POST attacks

### 4. **CGI/Script Execution Timeout**
**What**: Maximum time for CGI script to execute
**When**: During CGI processing
**Typical Value**: 30-60 seconds
**Current in your code**: 5000 ms (5 seconds) ✅

### 5. **Response Write Timeout**
**What**: Time to send response to client
**When**: During response transmission
**Typical Value**: 30-60 seconds
**Purpose**: Detect dead/slow clients

### 6. **Keep-Alive Timeout**
**What**: How long to keep connection open for reuse
**When**: After completing a request
**Typical Value**: 5-15 seconds
**Purpose**: Allow connection reuse while freeing resources

---

## Your Current Implementation

### ✅ What You Have:
```cpp
class Client {
    time_t _lastActivity;           // Tracks last activity
    static const int CGI_TIMEOUT = 5000;  // 5 seconds for CGI
    bool hasTimedOut(time_t now);   // Checks CGI timeout only
    void updateActivity();          // Updates timestamp
};
```

### ❌ What's Missing:
1. **No timeout checking in main loop** - `hasTimedOut()` is never called
2. **Only CGI timeout defined** - No client read/write timeouts
3. **No timeout for header/body reading**
4. **No timeout for response writing**

---

## Recommended Implementation

### Step 1: Define Timeout Constants
```cpp
class Client {
private:
    static const time_t HEADER_TIMEOUT = 20;      // 20 seconds to receive headers
    static const time_t BODY_TIMEOUT = 120;       // 2 minutes for request body
    static const time_t CGI_TIMEOUT = 30;         // 30 seconds for CGI (not 5000!)
    static const time_t WRITE_TIMEOUT = 60;       // 1 minute to send response
    static const time_t KEEPALIVE_TIMEOUT = 10;   // 10 seconds between requests
    
    time_t _connectionStartTime;    // When connection was established
    time_t _lastActivity;           // Last read/write activity
    ClientState _state;
```

**Note**: Your `CGI_TIMEOUT = 5000` looks like milliseconds, but `time_t` typically uses seconds!

### Step 2: Update Timeout Check Logic
```cpp
bool Client::hasTimedOut(time_t now) {
    time_t elapsed = now - _lastActivity;
    
    switch (_state) {
        case READING_HEADERS:
            // Timeout if headers not received within 20 seconds
            return elapsed > HEADER_TIMEOUT;
            
        case READING_BODY:
            // Timeout if body not received within 2 minutes
            return elapsed > BODY_TIMEOUT;
            
        case CGI_PROCESSING:
            // Timeout if CGI takes more than 30 seconds
            return elapsed > CGI_TIMEOUT;
            
        case SENDING_RESPONSE:
            // Timeout if can't send response within 1 minute
            return elapsed > WRITE_TIMEOUT;
            
        case REQUEST_READY:
        case DONE:
            // Keep-alive timeout: 10 seconds between requests
            return elapsed > KEEPALIVE_TIMEOUT;
            
        default:
            return false;
    }
}
```

### Step 3: Check Timeouts in Main Loop
```cpp
int Webserv::connectAndRead(void) {
    std::vector<int> removeFds;
    time_t now = time(NULL);  // Get current time once per loop
    
    // Check all clients for timeouts
    for (std::map<int, Client*>::iterator it = _clients.begin(); 
         it != _clients.end(); ++it) {
        Client *client = it->second;
        
        if (client->hasTimedOut(now)) {
            std::stringstream msg;
            msg << "Client #" << client->getFd() 
                << " timed out in state " << client->getStateString();
            printLog(YELLOW, "TIMEOUT", msg.str());
            
            removeFds.push_back(client->getFd());
            continue;  // Skip processing this client
        }
    }
    
    // ... rest of your event processing ...
    
    // Remove timed-out clients
    for (size_t i = 0; i < removeFds.size(); ++i)
        disconnectClient(removeFds[i]);
}
```

### Step 4: Update Activity Timestamps
```cpp
// Update in readAndParseRequest()
int Client::readAndParseRequest() {
    updateActivity();  // ✅ Update on every read
    // ... read logic ...
}

// Update in writeResponse()
int Client::writeResponse() {
    updateActivity();  // ✅ Update on every write
    // ... write logic ...
}

// Update in CGI event handling
void CgiHandler::handleEvent(...) {
    _client->updateActivity();  // ✅ Update on CGI I/O
    // ... CGI handling ...
}
```

---

## HTTP/1.1 Specification Requirements

According to **RFC 7230** and **RFC 2616**:

1. **Connection Timeout**: Not strictly required, but recommended
2. **408 Request Timeout**: Should send this status code when timeout occurs
3. **Keep-Alive**: Should close after timeout on persistent connections

### Sending 408 Timeout Response
```cpp
void Client::sendTimeoutResponse() {
    std::string response = 
        "HTTP/1.1 408 Request Timeout\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "Content-Length: 52\r\n"
        "\r\n"
        "<html><body><h1>408 Request Timeout</h1></body></html>";
    
    // Try to send, but don't wait - just close if it fails
    send(_fd, response.c_str(), response.length(), MSG_DONTWAIT);
}
```

---

## Common Web Server Timeout Values

| Server | Read Timeout | Write Timeout | Keep-Alive | CGI Timeout |
|--------|--------------|---------------|------------|-------------|
| Nginx | 60s | 60s | 75s | 60s |
| Apache | 60s | 60s | 5s | 300s |
| Your Server | **Recommended** | **Recommended** | **Recommended** | **5s → 30s** |

---

## Practical Example: Full Implementation

```cpp
// In Client.hpp
class Client {
private:
    static const time_t HEADER_TIMEOUT = 20;
    static const time_t BODY_TIMEOUT = 120;
    static const time_t CGI_TIMEOUT = 30;        // Fix: was 5000 (wrong unit!)
    static const time_t WRITE_TIMEOUT = 60;
    static const time_t KEEPALIVE_TIMEOUT = 10;
    
    time_t _lastActivity;
    // ...
};

// In Client.cpp
void Client::updateActivity() {
    _lastActivity = time(NULL);
}

bool Client::hasTimedOut(time_t now) {
    time_t elapsed = now - _lastActivity;
    
    switch (_state) {
        case READING_HEADERS: return elapsed > HEADER_TIMEOUT;
        case READING_BODY:    return elapsed > BODY_TIMEOUT;
        case CGI_PROCESSING:  return elapsed > CGI_TIMEOUT;
        case SENDING_RESPONSE:return elapsed > WRITE_TIMEOUT;
        case REQUEST_READY:
        case DONE:            return elapsed > KEEPALIVE_TIMEOUT;
        default:              return false;
    }
}

// In WebservRunner.cpp
int Webserv::runningServ(void) {
    int timeout = 1000;  // poll() timeout in milliseconds
    
    while (g_running) {
        status = poll(_pollFds.data(), _pollFds.size(), timeout);
        
        if (status == -1) {
            if (!g_running) break;
            return handleFunctionError("poll");
        }
        
        if (status == 0) {
            checkClientTimeouts();  // ✅ Check timeouts when poll returns 0
            continue;
        }
        
        if (connectAndRead() < 0)
            return 1;
    }
    return 0;
}

void Webserv::checkClientTimeouts() {
    time_t now = time(NULL);
    std::vector<int> timeoutFds;
    
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it) {
        if (it->second->hasTimedOut(now)) {
            timeoutFds.push_back(it->first);
        }
    }
    
    for (size_t i = 0; i < timeoutFds.size(); ++i) {
        std::cerr << "Client " << timeoutFds[i] << " timed out" << std::endl;
        disconnectClient(timeoutFds[i]);
    }
}
```

---

## Testing Timeouts

### Test Header Timeout (20 seconds)
```bash
# Connect but don't send anything
telnet localhost 8080
# Wait 21 seconds → should disconnect
```

### Test CGI Timeout (30 seconds)
```python
#!/usr/bin/env python3
import time
print("Content-Type: text/plain\n")
time.sleep(35)  # Exceeds CGI_TIMEOUT
print("This should timeout")
```

### Test Slow Read (Body Timeout)
```bash
# Send headers, then slowly send body
(echo -ne "POST /upload HTTP/1.1\r\nContent-Length: 100\r\n\r\n"; \
 sleep 130; echo "data") | nc localhost 8080
```

---

## Summary

### Should You Implement Timeouts?
**YES!** For production, timeouts are essential.

### Minimum Implementation (Quick Fix):
1. Fix `CGI_TIMEOUT = 5000` → `CGI_TIMEOUT = 30` (use seconds, not milliseconds)
2. Add `checkClientTimeouts()` called from main loop
3. Call `updateActivity()` in read/write functions

### Full Implementation (Recommended):
1. Define all 5 timeout types
2. Implement state-based timeout checking
3. Send 408 responses when appropriate
4. Log timeout events
5. Test with slow client tools

### What Happens Without Timeouts?
- 10 slow clients = 10 connections stuck forever
- 1000 slow clients = server dead (DoS)
- Memory grows indefinitely
- No way to clean up zombie connections

Would you like me to help you implement the timeout checking in your code?
