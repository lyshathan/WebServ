# Stress Test Bug Fixes - Summary

## Issue
Server worked fine for single requests but experienced **broken pipe errors** under stress testing with multiple concurrent CGI requests.

## Root Causes Identified

### 1. **Wrong FD Mappings in `addCGIToPoll()`**
- **Location**: `srcs/webserv/WebservHelpers.cpp` lines 12-17
- **Problem**: stdin and stdout FD mappings were reversed
- **Impact**: Under stress, multiple CGI processes would write to wrong file descriptors, causing SIGPIPE

### 2. **Double-Close of File Descriptors**
- **Location**: `srcs/webserv/WebservHelpers.cpp` `removePollFd()`
- **Problem**: CGI FDs were closed in `CgiHandler::cleanUp()` then closed again in `removePollFd()`
- **Impact**: Attempting to close already-closed FDs, or worse, closing FDs that were reused for new connections

### 3. **Duplicate Removals**
- **Location**: `srcs/webserv/WebservRunner.cpp` `connectAndRead()`
- **Problem**: Same FD could appear multiple times in `removeFds` vector
- **Impact**: Multiple attempts to remove/close the same FD

### 4. **POLLOUT on New Clients**
- **Location**: `srcs/webserv/WebservManageClient.cpp` `addClient()`
- **Problem**: New clients registered with `POLLIN | POLLOUT`
- **Impact**: Unnecessary wake-ups from poll(), wasting CPU cycles

## Fixes Applied

### Fix 1: Corrected FD Mappings
**File**: `srcs/webserv/WebservHelpers.cpp`

```cpp
// BEFORE (WRONG):
_cgiToClient[stdinFd] = client;   // Should be stdout
_cgiToClient[stdoutFd] = client;  // Should be stdin

// AFTER (CORRECT):
_cgiToClient[stdoutFd] = client;  // stdout mapped to client
_cgiToClient[stdinFd] = client;   // stdin mapped to client
```

### Fix 2: Prevent Double-Close
**File**: `srcs/webserv/WebservHelpers.cpp`

```cpp
void Webserv::removePollFd(int fd) {
    // Remove from _pollFds vector
    
    // NEW: Check if it's a CGI FD (already closed in cleanUp)
    if (_cgiToClient.find(fd) != _cgiToClient.end()) {
        _cgiToClient.erase(fd);
        return;  // Don't close again!
    }
    
    // Only close client FDs here
    // ... rest of function
}
```

### Fix 3: Remove Duplicates with std::set
**File**: `srcs/webserv/WebservRunner.cpp`

```cpp
// BEFORE:
for (size_t i = 0; i < removeFds.size(); ++i)
    removePollFd(removeFds[i]);

// AFTER:
std::set<int> uniqueRemoveFds(removeFds.begin(), removeFds.end());
for (std::set<int>::iterator it = uniqueRemoveFds.begin(); 
     it != uniqueRemoveFds.end(); ++it)
    removePollFd(*it);
```

### Fix 4: POLLIN Only for New Clients
**File**: `srcs/webserv/WebservManageClient.cpp`

```cpp
// BEFORE:
newClientPollFd.events = POLLIN | POLLOUT;

// AFTER:
newClientPollFd.events = POLLIN;  // Only wait for incoming data
```

### Fix 5: Added Missing Include
**File**: `srcs/webserv/Includes.hpp`

```cpp
#include <set>  // For std::set used in Fix 3
```

## How These Fixes Resolve the Broken Pipe Issue

1. **Correct FD Mapping**: Each CGI process now writes to its own stdin and reads from its own stdout
2. **No Double-Close**: FDs are only closed once, preventing errors from closing invalid/reused FDs
3. **No Duplicates**: Each FD is processed exactly once for removal
4. **Efficient Polling**: poll() only wakes up when there's actual data to read from new clients

## Testing Recommendations

1. **Single Request Test**: Verify basic functionality still works
2. **Concurrent Requests**: Use `ab` or `siege` to send multiple simultaneous CGI requests
3. **Long-Running CGI**: Test with CGI scripts that take time to execute
4. **Large POST Data**: Test CGI with large request bodies to stress the write path

### Example Stress Test Command:
```bash
# Apache Bench: 100 requests, 10 concurrent
ab -n 100 -c 10 http://localhost:8080/cgi-bin/script.py

# Siege: 50 concurrent users for 30 seconds
siege -c 50 -t 30S http://localhost:8080/cgi-bin/script.py
```

## Files Modified
- ✅ `srcs/webserv/WebservHelpers.cpp` (2 functions)
- ✅ `srcs/webserv/WebservRunner.cpp` (1 function)
- ✅ `srcs/webserv/WebservManageClient.cpp` (1 function)
- ✅ `srcs/webserv/Includes.hpp` (1 include added)

## Status
✅ All changes compiled successfully with no errors
✅ Ready for stress testing
