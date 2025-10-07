#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print("Content-Length: 4523")
print("Cache-Control: no-cache")
print("X-Powered-By: WebServ-CGI")
print()

# HTML page
print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Environment - Webserv</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: #f5f5f7;
            color: #1d1d1f;
            padding: 40px 20px;
        }
        .container {
            max-width: 980px;
            margin: 0 auto;
        }
        h1 {
            font-size: 40px;
            font-weight: 600;
            margin-bottom: 12px;
        }
        .subtitle {
            font-size: 17px;
            color: #6e6e73;
            margin-bottom: 40px;
        }
        .card {
            background: white;
            border-radius: 18px;
            padding: 32px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.07);
            margin-bottom: 20px;
        }
        h2 {
            font-size: 24px;
            font-weight: 600;
            margin-bottom: 16px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            text-align: left;
            padding: 12px;
            border-bottom: 1px solid #f5f5f7;
        }
        th {
            font-weight: 600;
            color: #1d1d1f;
        }
        td {
            font-family: 'SF Mono', Monaco, monospace;
            font-size: 14px;
            color: #6e6e73;
            word-break: break-all;
        }
        .body-content {
            background: #f5f5f7;
            padding: 16px;
            border-radius: 8px;
            font-family: 'SF Mono', Monaco, monospace;
            font-size: 14px;
            white-space: pre-wrap;
            word-break: break-all;
        }
        .empty {
            color: #6e6e73;
            font-style: italic;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>CGI Environment</h1>
        <p class="subtitle">Environment variables and request body</p>

        <div class="card">
            <h2>Environment Variables</h2>
            <table>
                <tr>
                    <th>Variable</th>
                    <th>Value</th>
                </tr>
""")

# Print all environment variables
for key, value in sorted(os.environ.items()):
    print(f"                <tr><td>{key}</td><td>{value}</td></tr>")

print("""            </table>
        </div>

        <div class="card">
            <h2>Request Body</h2>
""")

# Read and print request body if present
content_length = os.environ.get('CONTENT_LENGTH', '')
if content_length and content_length.isdigit():
    body = sys.stdin.read(int(content_length))
    if body:
        print(f'            <div class="body-content">{body}</div>')
    else:
        print('            <div class="empty">No body content</div>')
else:
    print('            <div class="empty">No body content</div>')

print("""        </div>
    </div>
</body>
</html>""")