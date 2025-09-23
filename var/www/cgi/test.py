#!/usr/bin/env python3

import sys
import os

# Force unbuffered output
sys.stdout.reconfigure(line_buffering=True)

print("Content-Type: text/plain")
print("")  # Empty line required between headers and body
print("Hello from CGI test script!")
print("REQUEST_METHOD:", os.environ.get('REQUEST_METHOD', 'Not set'))
print("CONTENT_LENGTH:", os.environ.get('CONTENT_LENGTH', 'Not set'))

sys.stdout.flush()