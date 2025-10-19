# Webserv Test Checklist

## Config File Tests

### Already Done ✓
- [x] Empty file
- [x] Bad formatting (missing semi-colons)
- [x] Error_page with invalid value
- [x] Invalid directives
- [x] Duplicate directives

### Additional Tests
- [x] Duplicate ports in same server block
  ```nginx
  server {
      listen 8080;
      listen 8080;  # Should error
  }
  ```
- [x] Port 0 or > 65535
  ```nginx
  server {
      listen 0;      # Invalid
      listen 65536;  # Too high
      listen 99999;  # Way too high
  }
  ```
- [x] Negative client_max_body_size
  ```nginx
  server {
      client_max_body_size -100;  # Should error
  }
  ```
- [] Missing mandatory directives (server_name, listen, etc.) <- Test if server_name is mandatory ?
  ```nginx
  server {
      # Missing listen and server_name
      root /var/www;
  }
  ```
- [x] CGI extension without path
  ```nginx
  location / {
      cgi_ext .php;  # Missing cgi_path
  }
  ```
- [ ] Root directory that doesn't exist <- should start or not?
  ```nginx
  server {
      root /this/path/does/not/exist;
  }
  ```

---

## HTTP Request Tests

### Already Done ✓
- [x] Unauthorized method
- [x] Invalid method <- should have a different error code for when method does not exist?
- [x] 404 pages
- [x] 403 requests

### Additional Tests
- [x] `POST` with body > client_max_body_size
  ```bash
  # If max is 1000 bytes
  curl -X POST -d "$(python3 -c 'print("X"*2000)')" http://localhost:8080/
  # Should return 413 Payload Too Large
  ```
- [x] `POST` with `Content-Length` mismatch
  ```bash
  printf "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 100\r\n\r\nShortBody" | nc localhost 8080 ->Check this, why does it not time out?
  # Body is shorter than Content-Length says
  ```
- [x] `GET` with query strings (`?param=value`)
  ```bash
  curl "http://localhost:8080/page?name=john&age=25"
  ```
- [ ] Multiple consecutive requests on same connection (keep-alive)
  ```bash
  printf "GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
  ```
- [ ] Incomplete request (send half, wait, send rest)
  ```bash
  (echo -n "GET / HTTP/1.1\r\nHost: localhost"; sleep 2; echo -e "\r\n\r\n") | nc localhost 8080
  ```
- [x] Request with no `\r\n\r\n` ending
  ```bash
  printf "GET / HTTP/1.1\r\nHost: localhost\r\n" | nc localhost 8080
  # Hangs or times out
  ```
- [ ] Very long URL (2000+ chars)
  ```bash
  curl "http://localhost:8080/$(python3 -c 'print("a"*3000)')"
  # Should return 414 URI Too Long
  ```
- [ ] Invalid HTTP version (`HTTP/2.0`, `HTTP/0.9`)
  ```bash
  printf "GET / HTTP/2.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080
  # Should return 505 HTTP Version Not Supported
  ```
- [ ] Missing Host header
  ```bash
  printf "GET / HTTP/1.1\r\n\r\n" | nc localhost 8080
  # Should return 400 Bad Request
  ```
- [x] `DELETE` on non-existent file
  ```bash
  curl -X DELETE http://localhost:8080/does_not_exist.txt
  # Should return 404 Not Found
  ```

---

## Edge Cases

- [x] Request a directory without trailing `/` (should redirect to `/`)
  ```bash
  curl -I http://localhost:8080/folder
  # Should return 301 and Location: /folder/
  ```
- [x] Request a directory with autoindex OFF and no index file
  ```bash
  # Config: autoindex off; (no index.html in directory)
  curl http://localhost:8080/empty_dir/
  # Should return 403 Forbidden or 404
  ```
- [ ] Upload file to non-writable location
  ```bash
  curl -X POST --data-binary @file.txt http://localhost:8080/readonly/
  # Should return 403 Forbidden
  ```
- [ ] CGI that takes too long (timeout?)
  ```php
  <?php sleep(60); echo "Done"; ?>
  # Should timeout and return 504 Gateway Timeout
  ```
- [ ] CGI that returns invalid headers
  ```python
  #!/usr/bin/env python3
  print("Not a valid header")
  # Missing Content-Type, etc.
  ```
- [x] Multiple slashes in path (`//folder///file`)
  ```bash
  curl http://localhost:8080///folder//file.html
  # Should normalize to /folder/file.html
  ```
- [ ] Request with spaces in URL (`/my%20file.html`)
  ```bash
  curl http://localhost:8080/my%20file.html
  # Should decode %20 to space
  ```
- [ ] Absolute path in URL (`GET http://localhost:8080/path`)
  ```bash
  printf "GET http://localhost:8080/page HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
  # Should handle absolute URI
  ```

---

## Stress Tests

### Basic Load
```bash
# Many simultaneous connections
siege -c 100 -r 10 http://localhost:8080/
```

### Large File Upload
```bash
# Create 10MB file
dd if=/dev/zero of=big.bin bs=1M count=10

# Upload it
curl -X POST --data-binary @big.bin http://localhost:8080/upload
```

### Infinite Siege
```bash
# Should run indefinitely without crashes/leaks
siege -b http://localhost:8080/
```

### Memory Check
```bash
# Monitor memory while running stress tests
# Memory usage should NOT increase indefinitely
watch -n 1 'ps aux | grep webserv'
```

---

## CGI Tests

- [ ] Valid CGI execution (.php, .py, etc.)
  ```bash
  # test.php: <?php echo "Hello from PHP"; ?>
  curl http://localhost:8080/test.php
  ```
- [ ] CGI with POST data
  ```bash
  curl -X POST -d "name=john&age=25" http://localhost:8080/form.php
  ```
- [ ] CGI with query strings
  ```bash
  curl "http://localhost:8080/script.py?user=alice&id=123"
  ```
- [ ] CGI that outputs to stdout
  ```python
  #!/usr/bin/env python3
  print("Content-Type: text/html\n")
  print("<h1>Hello</h1>")
  ```
- [ ] CGI with invalid shebang
  ```python
  #!/invalid/path/python3
  print("This won't work")
  ```
- [ ] CGI file without execute permissions
  ```bash
  chmod 644 script.py  # Remove execute bit
  curl http://localhost:8080/script.py
  # Should return 403 or 500
  ```
- [ ] CGI that doesn't exist
  ```bash
  curl http://localhost:8080/missing.php
  # Should return 404
  ```

---

## Telnet/Curl Examples

### Incomplete request
```bash
telnet localhost 8080
GET /index.html HTTP/1.1
Host: localhost
# (wait here, don't send \r\n\r\n)
```

### No ending
```bash
printf "GET / HTTP/1.1\r\nHost: localhost\r\n" | nc localhost 8080
```

### Very long URL
```bash
curl http://localhost:8080/$(python3 -c "print('a'*3000)")
```

### Body size exceeded
```bash
curl -X POST -H "Content-Type: text/plain" \
  --data "$(python3 -c "print('X'*10000000)")" \
  http://localhost:8080/upload
```

---

## Browser Tests

- [ ] Open in Chrome/Firefox
- [ ] Check Network tab for proper headers
- [ ] Serve static website (HTML, CSS, JS, images)
- [ ] Wrong URL (404 page)
- [ ] Directory listing
- [ ] Follow redirects
- [ ] Upload file via HTML form



- Test if server_name is mandatory in NGINX its not
- Test difference between not allowed/inexistant method returns either 403 or 400
- Check problems with nc in my server
- Test url too long - returns 414 Request-URI Too Large
- Unsuported HTTP version should return 505 HTTP Version Not Supported
