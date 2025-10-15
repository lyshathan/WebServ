#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")
cookies_var = os.environ.get("HTTP_COOKIE")

if cookies_var is None:
    print("No cookies found")
    exit(0)

cookies = cookies_var.split(";")

print(f"found {len(cookies)} cookies")
for cookie in cookies:
	elt = cookie.split("=")
	print(f"cookie name: {elt[0]} => {elt[1]}")