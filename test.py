#!/usr/bin/env python3
import socket, requests, random, string, os

BASE = "http://localhost:8080"
HOST, PORT = "127.0.0.1", 8080
TIMEOUT = 3

def send_raw(raw):
    """Send raw HTTP request and return response string."""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(TIMEOUT)
    s.connect((HOST, PORT))
    s.sendall(raw.encode())
    try:
        resp = s.recv(4096).decode(errors='ignore')
    except socket.timeout:
        resp = "<timeout>"
    s.close()
    return resp

def show(name, ok, status, extra=""):
    print(f"{name:<35} {'✅' if ok else '❌'} (status={status}) {extra}")

# ---------------------------------------------------------
# BASIC TESTS (from your config)
# ---------------------------------------------------------
def basic_tests():
    print("\n=== BASIC TESTS ===")
    r = requests.get(f"{BASE}/index.html"); show("GET /index.html", r.status_code == 200, r.status_code)
    r = requests.get(f"{BASE}/notfound.html"); show("GET /notfound", r.status_code == 404, r.status_code)
    r = requests.post(f"{BASE}/"); show("POST / (not allowed)", r.status_code == 405, r.status_code)
    r = requests.get(f"{BASE}/directory/"); show("Autoindex /directory", r.status_code == 200, r.status_code)
    files = {"file": ("tmp.txt", b"upload test")}
    r = requests.post(f"{BASE}/uploads", files=files); show("POST /uploads", r.status_code in (200,201), r.status_code)
    r = requests.get(f"{BASE}/cgi/index.py"); show("CGI Python", r.status_code == 200, r.status_code)
    r = requests.get(f"{BASE}/redirection", allow_redirects=False); show("Redirection", r.status_code == 302, r.status_code)

# ---------------------------------------------------------
# EDGE CASE TESTS (low-level)
# ---------------------------------------------------------
def edge_tests():
    print("\n=== EDGE CASE TESTS ===")
    def check(name, raw, expected):
        resp = send_raw(raw)
        ok = str(expected) in resp
        status = resp.splitlines()[0] if resp else "NO RESP"
        show(name, ok, status)

    check("Invalid method", "GETTT / HTTP/1.1\r\nHost: localhost\r\n\r\n", 405)
    check("Missing Host header", "GET / HTTP/1.1\r\n\r\n", 400)
    check("Unsupported version", "GET / HTTP/2.0\r\nHost: localhost\r\n\r\n", 504)
    check("Header without colon", "GET / HTTP/1.1\r\nHost: localhost\r\nFooBar\r\n\r\n", 400)
    check("Empty request line", "\r\n", 400)
    check("Invalid chunk size", "POST /uploads HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\nZZ\r\nX\r\n", 400)
    check("Unsupported method TRACE", "TRACE / HTTP/1.1\r\nHost: localhost\r\n\r\n", 405)
    check("Double Content-Length", "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\nContent-Length: 10\r\n\r\n", 400)
    check("Missing body", "POST /uploads HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\n\r\n", 400)
    long_url = "/" + "a"*9000
    check("Long URI", f"GET {long_url} HTTP/1.1\r\nHost: localhost\r\n\r\n", 414)

    # optional: CGI timeout (simulate long script)
    slow_req = "GET /cgi/timeout.sh HTTP/1.1\r\nHost: localhost\r\n\r\n"
    resp = send_raw(slow_req)
    ok = "504" in resp or "Gateway" in resp or "timeout" in resp
    show("CGI timeout (if implemented)", ok, resp.splitlines()[0] if resp else "NO RESP")

# ---------------------------------------------------------
def main():
    basic_tests()
    edge_tests()

if __name__ == "__main__":
    main()
