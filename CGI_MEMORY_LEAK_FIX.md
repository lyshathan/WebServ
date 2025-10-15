# CGI Memory Leak and Crash Fixes

## Issues Found During Stress Testing

### 1. **Memory Leak: CgiHandler Not Deleted**
**Symptom**: LeakSanitizer detected 240 bytes leaked per CGI request
```
Direct leak of 240 byte(s) in 1 object(s) allocated from:
    #1 0x54809d in Client::launchCGI()
```

**Root Cause**:
- `Client::_cgi` was never initialized (contained garbage value)
- `Client` destructor didn't delete the `_cgi` object
- When a client was deleted, the CGI object was leaked

**Fix Applied** (`srcs/parsing/Client.cpp`):
```cpp
// Constructor: Initialize _cgi to NULL
Client::Client(...) : ..., _cgi(NULL), ... {}

// Destructor: Delete CGI if it exists
Client::~Client() {
    if (_cgi) {
        delete _cgi;
        _cgi = NULL;
    }
    delete httpReq;
    delete httpRes;
}
```

---

### 2. **Dangling CGI FDs When Client Disconnects**
**Symptom**: 
- CGI FDs remained in `_pollFds` after client disconnected
- Attempts to process events for non-existent client
- "[DEBUG] Entering removePollFd for fd X" called multiple times for same FD

**Root Cause**:
When a client disconnected while CGI was processing:
1. Client FD removed and client deleted
2. But CGI stdin/stdout FDs still in `_pollFds` array
3. `_cgiToClient` map still points to deleted client
4. Next poll() triggers events on orphaned CGI FDs
5. Code tries to access deleted client → undefined behavior

**Fix Applied** (`srcs/webserv/WebservHelpers.cpp`):
```cpp
void Webserv::removePollFd(int fd) {
    // ... remove from _pollFds ...
    
    // If it's a CGI FD, just remove from map (already closed)
    if (_cgiToClient.find(fd) != _cgiToClient.end()) {
        _cgiToClient.erase(fd);
        return;
    }
    
    // If it's a client FD with active CGI
    std::map<int, Client*>::iterator clientIt = _clients.find(fd);
    if (clientIt != _clients.end()) {
        Client *client = clientIt->second;
        
        // Clean up CGI FDs BEFORE deleting client
        if (client->getCgi()) {
            CgiHandler *cgi = client->getCgi();
            int stdinFd = cgi->getStdinFd();
            int stdoutFd = cgi->getStdoutFd();
            
            // Remove from map
            if (stdinFd > 0) _cgiToClient.erase(stdinFd);
            if (stdoutFd > 0) _cgiToClient.erase(stdoutFd);
            
            // Remove from _pollFds
            for (iterator it = _pollFds.begin(); it != _pollFds.end(); ) {
                if (it->fd == stdinFd || it->fd == stdoutFd) {
                    it = _pollFds.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        delete client;  // Now safe to delete
        _clients.erase(clientIt);
    }
    
    close(fd);
}
```

---

### 3. **CGI Script Crash: Missing HTTP_COOKIE**
**Symptom**: 
```
AttributeError: 'NoneType' object has no attribute 'split'
[CGI ERROR] CGI exited abnormally
```

**Root Cause**:
- CGI script assumed `HTTP_COOKIE` environment variable always exists
- When no cookies sent, `os.environ.get("HTTP_COOKIE")` returns `None`
- Calling `.split()` on `None` → crash

**Fix Applied** (`var/www/cgi/test.py`):
```python
cookies_var = os.environ.get("HTTP_COOKIE")

if cookies_var is None:
    print("No cookies found")
    exit(0)

cookies = cookies_var.split(";")
```

---

## Impact of Fixes

### Before:
- ✗ 50% availability (143 failed transactions out of 286 total)
- ✗ All transactions failed
- ✗ Memory leak of 240 bytes per request
- ✗ CGI scripts crashing with AttributeError
- ✗ Dangling FD references causing undefined behavior

### After (Expected):
- ✓ No memory leaks
- ✓ CGI scripts handle missing environment variables gracefully
- ✓ Proper cleanup when client disconnects during CGI processing
- ✓ No dangling FD references
- ✓ Higher success rate under stress testing

---

## Files Modified
1. ✅ `srcs/parsing/Client.cpp` - Fixed constructor and destructor
2. ✅ `srcs/webserv/WebservHelpers.cpp` - Enhanced client cleanup
3. ✅ `var/www/cgi/test.py` - Handle missing cookies gracefully

---

## Testing

### Quick Test:
```bash
# Single request (should work now)
curl http://localhost:8080/cgi/test.py

# Stress test (should have better success rate)
siege -c5 -t5s http://localhost:8080/cgi/test.py

# Check for memory leaks (should be clean now)
# Run server, send requests, CTRL+C, check LeakSanitizer output
```

### What to Monitor:
1. No LeakSanitizer errors on shutdown
2. No "AttributeError" from Python scripts
3. No "[CGI ERROR] CGI exited abnormally" (unless script genuinely fails)
4. Higher success rate in siege output
5. No crashes or hangs

---

## Additional Notes

### Why PID was -1:
The cleanup was being called after `waitpid()` already collected the process, or the process was already cleaned up. The fixes above ensure proper ordering.

### Key Learning:
When managing resources with poll():
1. Always initialize pointers to NULL
2. Clean up child resources BEFORE deleting parent
3. Remove ALL references to a resource before considering it "cleaned up"
4. Handle missing environment variables in CGI scripts gracefully
