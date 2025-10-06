#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html\r\n")
print("<html><body>")
print("<h1>CGI Test</h1>")

print("<h2>Environment Variables:</h2>")
for key, value in sorted(os.environ.items()):
    print(f"<p>{key} = {value}</p>")

print("<h2>STDIN Content:</h2>")
content_length = os.environ.get('CONTENT_LENGTH', '0')
print(f"<p>CONTENT_LENGTH = '{content_length}'</p>")

if content_length and content_length.isdigit() and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))
    print(f"<p>Body received: '{body}'</p>")
    print(f"<p>Body length: {len(body)}</p>")
else:
    print("<p>No content length or zero length</p>")

# Try to read whatever is available on stdin
try:
    import select
    if select.select([sys.stdin], [], [], 0)[0]:
        extra = sys.stdin.read()
        print(f"<p>Extra stdin data: '{extra}'</p>")
    else:
        print("<p>No additional stdin data available</p>")
except:
    print("<p>Could not check for additional stdin data</p>")

print("</body></html>")