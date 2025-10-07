# Webserv Evaluation Checklist

## Preliminary Checks

### Code Review
- [ ] Ask: Which function for I/O Multiplexing? (poll)
- [ ] Ask: How does select/equivalent work?
- [ ] Ask: How do you handle server accept and client read/write with one select?
- [ ] **CRITICAL**: Only ONE select in main loop, checks read AND write simultaneously
- [ ] Only one read/write per client per select cycle
- [ ] All read/recv/write/send check return values (not just -1 or 0, should do both)
- [ ] NO errno checks after read/recv/write/send
- [ ] NO read/write without going through poll
- [ ] Compiles without re-link

## Configuration File Tests

### Multiple Servers
```bash
# Test different ports
curl http://localhost:8080
curl http://localhost:8081

# Test different hostnames
curl --resolve example.com:80:127.0.0.1 http://example.com/
```

### Error Pages
```bash
# Test custom 404
curl http://localhost:8080/nonexistent
```

### Body Size Limit
```bash
# Under limit
curl -X POST -H "Content-Type: text/plain" --data "short" http://localhost:8080/upload

# Over limit
curl -X POST -H "Content-Type: text/plain" --data "$(python3 -c 'print("A"*10000000)')" http://localhost:8080/upload
```

### Routes & Methods
```bash
# Test different directories
curl http://localhost:8080/images/
curl http://localhost:8080/files/

# Test default file in directory
curl http://localhost:8080/

# Test method restrictions
curl -X DELETE http://localhost:8080/file.txt
curl -X DELETE http://localhost:8080/protected/file.txt  # Should fail if not allowed
```

## Basic HTTP Methods

### GET
```bash
curl -v http://localhost:8080/index.html
```

### POST
```bash
curl -X POST -d "param1=value1&param2=value2" http://localhost:8080/form
```

### DELETE
```bash
curl -X DELETE http://localhost:8080/file.txt
```

### Unknown Method
```bash
curl -X INVALID http://localhost:8080/
# Should return proper error, no crash
```

### File Upload/Download
```bash
# Upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# Download
curl http://localhost:8080/uploads/test.txt
```

## Browser Tests

- [ ] Open in browser, check Network tab
- [ ] Verify request/response headers
- [ ] Serve static website
- [ ] Try wrong URL (404)
- [ ] Try directory listing
- [ ] Try redirected URL

## Port Configuration

```bash
# Test: Same port twice in config should fail to start
# Test: Multiple servers with different configs on same port should handle correctly
```

## Stress Tests

### Siege
```bash
# Install
brew install siege  # macOS
# or apt-get install siege  # Linux

# Simple test
siege -b -t30s http://localhost:8080/

# Check:
# - Availability > 99.5%
# - No memory leaks (monitor with top/htop)
# - No hanging connections
# - Can run indefinitely
```

### Telnet Tests
```bash
telnet localhost 8080
GET / HTTP/1.1
Host: localhost

# Should return proper response
```

## Status Codes
Verify correct codes for:
- [ ] 200 OK
- [ ] 404 Not Found
- [ ] 405 Method Not Allowed
- [ ] 413 Payload Too Large
- [ ] 500 Internal Server Error
- [ ] 301/302 Redirects

## Bonus

### Cookies & Sessions
```bash
curl -v -c cookies.txt -b cookies.txt http://localhost:8080/login
```

### Multiple CGI
- [ ] Test .php files
- [ ] Test .py files (or other)

## Red Flags (Auto-fail)

- ❌ Select not checking read AND write simultaneously
- ❌ Multiple select calls
- ❌ errno check after read/write/send/recv
- ❌ Read/write without select
- ❌ Crash on any request
- ❌ Memory leaks
- ❌ Segfault