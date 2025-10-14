#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")
cookies_var = os.environ.get("HTTP_COOKIE")

cookies = cookies_var.split(";")

print(f"found {len(cookies)} cookies")
for cookie in cookies:
	elt = cookie.split("=")
	print(f"cookie name: {elt[0]} => {elt[1]}")