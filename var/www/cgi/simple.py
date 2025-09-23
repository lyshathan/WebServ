#!/usr/bin/env python3

import os
import sys

# --- Check for POST data ---
content_length = int(os.environ.get('CONTENT_LENGTH', 0))

if content_length > 0:
    post_data = sys.stdin.read(content_length)
    print(f"Received POST data: {post_data}")
else:
    print("Hello from Python CGI - No POST data received")

print("This is just the body content.")
print("Environment variables count:", len(os.environ))