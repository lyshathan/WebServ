#!/usr/bin/env python3

import os
import html

def main():
    """Simple CGI script to display environment variables."""
    
    # Get all environment variables
    env_vars = dict(os.environ)
    
    # Create simple HTML
    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Environment Variables - Simple View</title>
    <link rel="stylesheet" href="/static/style.css">
    <style>
        .env-container {{
            max-width: 800px;
            margin: 2rem auto;
            padding: 2rem;
            background: white;
            border-radius: 10px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
        }}
        .env-var {{
            margin: 1rem 0;
            padding: 1rem;
            background: #f8f9fa;
            border-left: 4px solid #228B22;
            border-radius: 4px;
        }}
        .env-key {{
            font-weight: bold;
            color: #2c5530;
            margin-bottom: 0.5rem;
        }}
        .env-value {{
            font-family: monospace;
            color: #555;
            word-break: break-all;
            background: white;
            padding: 0.5rem;
            border-radius: 4px;
        }}
    </style>
</head>
<body>
    <header>
        <nav>
            <a href="/index.html" class="logo">🌿 GreenLeaf</a>
            <ul class="nav-links">
                <li><a href="/index.html">Home</a></li>
                <li><a href="/catalog/index.html">Plant Catalog</a></li>
                <li><a href="/uploads/">Upload</a></li>
                <li><a href="/cgi/">CGI Scripts</a></li>
            </ul>
        </nav>
    </header>

    <main class="container">
        <div class="env-container">
            <h1>🌍 Environment Variables</h1>
            <p>Environment variables captured by the CGI script ({len(env_vars)} total)</p>
            
            <div class="env-list">"""
    
    # Add environment variables (sorted)
    for key, value in sorted(env_vars.items()):
        safe_key = html.escape(key)
        safe_value = html.escape(value) if value else '<em>empty</em>'
        
        html_content += f"""
                <div class="env-var">
                    <div class="env-key">{safe_key}</div>
                    <div class="env-value">{safe_value}</div>
                </div>"""
    
    html_content += """
            </div>
        </div>
    </main>

    <footer>
        <p>&copy; 2025 GreenLeaf Plant Catalog. Simple environment display.</p>
    </footer>
</body>
</html>"""
    
    # Print HTTP headers (matching working hello.py format)
    print("Content-Type: text/html", end="\r\n")
    print(f"Content-Length: {len(html_content)}", end="\r\n")
    print("Status: 200 OK", end="\r\n")
    print(end="\r\n")  # blank line to signal start of body
    
    # Print the HTML content
    print(html_content)

if __name__ == "__main__":
    main()