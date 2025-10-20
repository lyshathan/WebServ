#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/plain\n")

try:
    # Attempt to open a file located one directory above, inside uploads/
    target_path = os.path.join("..", "uploads", "sample.txt")
    with open(target_path, "r") as f:
        content = f.read()

    print(f"[SUCCESS] Successfully read from {target_path}")
    print(f"File content:\n{content}")

except FileNotFoundError:
    print(f"[ERROR] Could not find {target_path}")
    print(f"Current working directory: {os.getcwd()}")
    sys.exit(1)
except Exception as e:
    print(f"[ERROR] Exception occurred: {e}")
    print(f"Current working directory: {os.getcwd()}")
    sys.exit(1)
