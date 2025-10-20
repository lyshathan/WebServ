# Webserv Automated Test Suite

Each test below is **self-contained** — you can copy and paste it directly into your terminal.  
All commands assume your server binary is named `webserv` and is running in the same directory.

Each block includes:
- 🏷️ **Tags** (category)
- 🧠 **Description**
- 💻 **Command** (runnable as-is)
- 📊 **Expected result** (HTTP status, log output)

---

## 🧩 CONFIGURATION TESTS

### 🧪 Test: Invalid Port
**Tags:** config, error  
**Description:** The server should refuse to start when configuration contains invalid ports (≤ 0 or > 65535).
```bash
cat > invalid_port.conf <<'EOF'
server {
    listen 70000;
}
EOF
./webserv invalid_port.conf
```
**Expected:** ❌ Error — `Invalid listen port: 70000`

---

### 🧪 Test: Duplicate Listen
**Tags:** config, duplicate, error  
**Description:** Two identical `listen` directives in the same block must fail.
```bash
cat > duplicate_listen.conf <<'EOF'
server {
    listen 8080;
    listen 8080;
}
EOF
./webserv duplicate_listen.conf
```
**Expected:** ❌ Error — `Duplicate listen directive on port 8080`

---

### 🧪 Test: Missing Mandatory Directive
**Tags:** config, error  
**Description:** Missing required directives (like `listen`) should produce an error.
```bash
cat > missing_server.conf <<'EOF'
server {
    root /var/www;
}
EOF
./webserv missing_server.conf
```
**Expected:** ❌ Error — `Missing mandatory directive: listen`

---

### 🧪 Test: Nonexistent Root Directory
**Tags:** config, warning  
**Description:** A missing root path should warn but not crash.
```bash
cat > bad_root.conf <<'EOF'
server {
    listen 8080;
    root /does/not/exist;
}
EOF
./webserv bad_root.conf
```
**Expected:** ⚠️ Warning — `Root directory does not exist`

---

## 🌐 HTTP BASIC TESTS

### 🧪 Test: GET Static File
**Tags:** http, get  
**Description:** Serve a simple static HTML file.
```bash
echo "<h1>Hello</h1>" > index.html
curl -i http://localhost:8080/index.html
```
**Expected:** ✅ 200 OK — `[INFO] GET /index.html -> 200 OK`

---

### 🧪 Test: POST Upload
**Tags:** http, post, upload  
**Description:** Upload a small file to the `/upload` endpoint.
```bash
echo 'data' > upload.txt
curl -X POST --data-binary @upload.txt http://localhost:8080/upload
```
**Expected:** ✅ 201 Created — `[INFO] POST /upload -> 201 Created`

---

### 🧪 Test: DELETE Existing File
**Tags:** http, delete  
**Description:** Delete an existing file.
```bash
echo "temp" > delete_me.txt
curl -X DELETE http://localhost:8080/delete_me.txt
```
**Expected:** ✅ 204 No Content — `[INFO] DELETE /delete_me.txt -> 204 No Content`

---

### 🧪 Test: Invalid Method
**Tags:** http, method, error  
**Description:** Unknown methods should return 405.
```bash
printf "BREW / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```
**Expected:** ❌ 405 Method Not Allowed — `[ERROR] Unsupported method: BREW`

---

## ⚠️ HTTP EDGE TESTS

### 🧪 Test: Missing Host Header
**Tags:** http, parsing, error  
**Description:** HTTP/1.1 requests must contain a Host header.
```bash
printf "GET / HTTP/1.1\r\n\r\n" | nc localhost 8080
```
**Expected:** ❌ 400 Bad Request — `[ERROR] Missing Host header`

---

echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n" | nc localhost 8080

### 🧪 Test: Invalid HTTP Version
**Tags:** http, parsing, error  
**Description:** Unsupported versions must return 505.
```bash
printf "GET / HTTP/2.0\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```
**Expected:** ❌ 505 HTTP Version Not Supported — `[ERROR] Unsupported HTTP version`

---

### 🧪 Test: URL Too Long
**Tags:** http, parsing, limit  
**Description:** Very long URLs should trigger 414.
```bash
curl -i "http://localhost:8080/$(python3 -c 'print("a"*3000)')"
```
**Expected:** ❌ 414 URI Too Long — `[ERROR] URI Too Long`

---

### 🧪 Test: POST Body Too Large
**Tags:** http, post, limit  
**Description:** Body exceeding `client_max_body_size` must return 413.
```bash
curl -X POST -H "Content-Type: text/plain" --data "$(python3 -c 'print("X"*2000000)')" http://localhost:8080/
```
**Expected:** ❌ 413 Payload Too Large — `[ERROR] Payload Too Large`

---

### 🧪 Test: Keep-Alive Requests
**Tags:** http, keepalive  
**Description:** Multiple requests on one TCP connection should all succeed.
```bash
printf "GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080
```
**Expected:** ✅ Both responses 200 OK — `[INFO] Processed 2 requests in one connection`

---

### 🧪 Test: Incomplete Request
**Tags:** http, timeout  
**Description:** Partial requests should timeout instead of hanging.
```bash
(echo -n "GET / HTTP/1.1\r\nHost: localhost"; sleep 2; echo -e "\r\n\r\n") | nc localhost 8080
```
**Expected:** ⚠️ Timeout — `[ERROR] Request timeout`

---

## 🧠 CGI TESTS

### 🧪 Test: Valid Python CGI
**Tags:** cgi, python  
**Description:** Simple Python CGI script should return valid headers.
```bash
mkdir -p cgi-bin
cat > cgi-bin/test.py <<'EOF'
#!/usr/bin/env python3
print("Content-Type: text/html\n")
print("<h1>Hello CGI</h1>")
EOF
chmod +x cgi-bin/test.py
curl -i http://localhost:8080/cgi-bin/test.py
```
**Expected:** ✅ 200 OK — `[INFO] Executed CGI /cgi-bin/test.py -> 200 OK`

---

### 🧪 Test: Invalid CGI Headers
**Tags:** cgi, error  
**Description:** Invalid CGI output must produce 502 Bad Gateway.
```bash
cat > cgi-bin/bad_header.py <<'EOF'
#!/usr/bin/env python3
print("NotAHeader")
EOF
chmod +x cgi-bin/bad_header.py
curl -i http://localhost:8080/cgi-bin/bad_header.py
```
**Expected:** ❌ 502 Bad Gateway — `[ERROR] Invalid CGI output header`

---

### 🧪 Test: CGI Timeout
**Tags:** cgi, timeout  
**Description:** Long-running CGI must timeout.
```bash
cat > cgi-bin/slow.py <<'EOF'
#!/usr/bin/env python3
import time; time.sleep(60)
print("Content-Type: text/plain\n\nDone")
EOF
chmod +x cgi-bin/slow.py
curl -i http://localhost:8080/cgi-bin/slow.py
```
**Expected:** ❌ 504 Gateway Timeout — `[ERROR] CGI execution timeout`

---

### 🧪 Test: CGI Permission Denied
**Tags:** cgi, permission  
**Description:** Non-executable CGI file should trigger 403.
```bash
cat > cgi-bin/no_exec.py <<'EOF'
#!/usr/bin/env python3
print("Content-Type: text/plain\n\nNoExec")
EOF
chmod 644 cgi-bin/no_exec.py
curl -i http://localhost:8080/cgi-bin/no_exec.py
```
**Expected:** ❌ 403 Forbidden — `[ERROR] Permission denied executing CGI`

---

## 🔥 STRESS TESTS

### 🧪 Test: Siege Load (100 Clients)
**Tags:** stress, siege  
**Description:** Simulate 100 concurrent clients to check uptime.
```bash
siege -c 100 -r 5 http://localhost:8080/
```
**Expected:** ✅ Availability > 99.5% — `[INFO] Siege completed successfully`

---

### 🧪 Test: Memory Stability
**Tags:** stress, memory  
**Description:** Ensure memory doesn’t leak or grow continuously.
```bash
watch -n 1 'ps -o pid,cmd,%mem --sort=-%mem | grep webserv'
```
**Expected:** ✅ Stable memory — `[INFO] Memory usage steady`

---

### 🧪 Test: Infinite Siege
**Tags:** stress, stability  
**Description:** Server must run indefinitely under siege without crashing.
```bash
siege -b http://localhost:8080/
```
**Expected:** ✅ Stable under continuous load — `[INFO] No crash detected`

---

## 🧭 MANUAL CHECKS

### 🧪 Test: Browser Check
**Tags:** manual, browser  
**Description:** Open `localhost:8080` in browser and verify static pages, 404, redirects, and autoindex.
```bash
# Open in Chrome/Firefox → Inspect → Network tab
```
**Expected:** 👀 Visual OK — `[INFO] Browser compatibility OK`

---

### 🧪 Test: Upload via HTML Form
**Tags:** manual, upload  
**Description:** Upload file through HTML form to verify multipart parsing.
```bash
# Use HTML form <form enctype="multipart/form-data" ...>
```
**Expected:** ✅ 201 Created — `[INFO] Form upload success`

