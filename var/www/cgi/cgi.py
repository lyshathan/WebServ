#!/usr/bin/env python3

import os
import sys
import html
import urllib.parse
import json

def parse_query_params():
    """Parse URL query parameters from QUERY_STRING environment variable."""
    query_string = os.environ.get('QUERY_STRING', '')
    if not query_string:
        return {}
    
    try:
        # Parse the query string into a dictionary
        params = urllib.parse.parse_qs(query_string)
        # Convert lists to single values (take first value if multiple)
        return {k: v[0] if v else '' for k, v in params.items()}
    except Exception:
        return {}

def filter_env_vars(env_vars, params):
    """Filter environment variables based on query parameters."""
    filtered_vars = env_vars.copy()
    
    # Handle search parameter
    search_term = params.get('search', '').lower()
    if search_term:
        filtered_vars = {k: v for k, v in filtered_vars.items() 
                        if search_term in k.lower() or search_term in str(v).lower()}
    
    # Handle filter parameter (filter by key prefix)
    filter_prefix = params.get('filter', '').upper()
    if filter_prefix:
        filtered_vars = {k: v for k, v in filtered_vars.items() 
                        if k.upper().startswith(filter_prefix)}
    
    # Handle specific key parameter
    key_param = params.get('key', '')
    if key_param:
        filtered_vars = {k: v for k, v in filtered_vars.items() 
                        if k == key_param}
    
    return filtered_vars

def create_json_response(env_vars, params):
    """Create a JSON response with environment variables."""
    filtered_vars = filter_env_vars(env_vars, params)
    
    response_data = {
        'total_count': len(env_vars),
        'filtered_count': len(filtered_vars),
        'query_params': params,
        'environment_variables': filtered_vars
    }
    
    return json.dumps(response_data, indent=2)

def create_html_page(params=None):
    """Create a styled HTML page displaying environment variables."""
    
    if params is None:
        params = {}
    
    # Get all environment variables
    env_vars = dict(os.environ)
    
    # Apply filters based on query parameters
    filtered_env_vars = filter_env_vars(env_vars, params)
    
    # Sort environment variables alphabetically
    sorted_env_vars = sorted(filtered_env_vars.items())
    
    # Get initial search value from URL parameters
    initial_search = params.get('search', '')
    
    # Create the HTML content
    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Environment Variables - GreenLeaf</title>
    <meta name="description" content="View current environment variables in a beautiful, plant-themed interface.">
    <link rel="stylesheet" href="/static/style.css">
    <style>
        .env-container {{
            background: white;
            border-radius: 10px;
            padding: 2rem;
            margin: 2rem 0;
            box-shadow: 0 4px 15px rgba(0,0,0,0.1);
        }}
        
        .env-header {{
            text-align: center;
            margin-bottom: 2rem;
            color: #2c5530;
        }}
        
        .env-stats {{
            display: flex;
            justify-content: space-around;
            margin-bottom: 2rem;
            flex-wrap: wrap;
            gap: 1rem;
        }}
        
        .stat-card {{
            background: linear-gradient(135deg, #228B22 0%, #32CD32 100%);
            color: white;
            padding: 1rem;
            border-radius: 8px;
            text-align: center;
            min-width: 150px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.1);
        }}
        
        .stat-number {{
            font-size: 2rem;
            font-weight: bold;
            display: block;
        }}
        
        .env-grid {{
            display: grid;
            gap: 1rem;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
        }}
        
        .env-var {{
            background: #f8f9fa;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            padding: 1rem;
            transition: all 0.3s ease;
        }}
        
        .env-var:hover {{
            border-color: #228B22;
            box-shadow: 0 2px 8px rgba(34, 139, 34, 0.2);
            transform: translateY(-2px);
        }}
        
        .env-key {{
            font-weight: bold;
            color: #2c5530;
            font-size: 1.1rem;
            margin-bottom: 0.5rem;
            display: flex;
            align-items: center;
            gap: 0.5rem;
        }}
        
        .env-key::before {{
            content: "🌱";
        }}
        
        .env-value {{
            color: #555;
            word-break: break-all;
            background: white;
            padding: 0.5rem;
            border-radius: 4px;
            border-left: 3px solid #228B22;
            font-family: 'Courier New', monospace;
            font-size: 0.9rem;
        }}
        
        .search-box {{
            width: 100%;
            padding: 1rem;
            margin-bottom: 1.5rem;
            border: 2px solid #e9ecef;
            border-radius: 8px;
            font-size: 1rem;
            transition: border-color 0.3s ease;
        }}
        
        .search-box:focus {{
            outline: none;
            border-color: #228B22;
        }}
        
        .no-results {{
            text-align: center;
            color: #666;
            font-style: italic;
            padding: 2rem;
        }}
        
        @media (max-width: 768px) {{
            .env-stats {{
                flex-direction: column;
            }}
            
            .env-grid {{
                grid-template-columns: 1fr;
            }}
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
            <div class="env-header">
                <h1>🌍 Environment Variables Garden</h1>
                <p>Explore the ecosystem of your server environment - where every variable helps your web server grow!</p>
            </div>
            
            <div class="env-stats">
                <div class="stat-card">
                    <span class="stat-number">{len(env_vars)}</span>
                    <span>Total Variables</span>
                </div>
                <div class="stat-card">
                    <span class="stat-number">{len(filtered_env_vars)}</span>
                    <span>Filtered Results</span>
                </div>
                <div class="stat-card">
                    <span class="stat-number">{len([v for v in filtered_env_vars.values() if v])}</span>
                    <span>Non-Empty Values</span>
                </div>
                <div class="stat-card">
                    <span class="stat-number">{len([k for k in filtered_env_vars.keys() if k.startswith('HTTP_')])}</span>
                    <span>HTTP Headers</span>
                </div>
            </div>
            
            <input type="text" class="search-box" id="searchBox" value="{html.escape(initial_search)}" placeholder="🔍 Search environment variables (e.g., PATH, USER, HTTP_...)">
            
            <div class="query-examples" style="margin-bottom: 1rem; padding: 1rem; background: #f8f9fa; border-radius: 8px; border-left: 3px solid #228B22;">
                <h3 style="margin-bottom: 0.5rem; color: #2c5530;">💡 URL Query Examples:</h3>
                <div style="font-family: monospace; font-size: 0.9rem;">
                    <a href="?search=PATH" style="color: #228B22;">?search=PATH</a> - Search for variables containing "PATH"<br>
                    <a href="?filter=HTTP_" style="color: #228B22;">?filter=HTTP_</a> - Show only HTTP headers<br>
                    <a href="?key=USER" style="color: #228B22;">?key=USER</a> - Show specific variable<br>
                    <a href="?format=json" style="color: #228B22;">?format=json</a> - Get JSON output<br>
                    <a href="?search=server&format=json" style="color: #228B22;">?search=server&format=json</a> - Combine parameters
                </div>
                {f'<div style="margin-top: 0.5rem; padding: 0.5rem; background: #e8f5e8; border-radius: 4px;"><strong>Current Query:</strong> <code>{html.escape(os.environ.get("QUERY_STRING", "none"))}</code></div>' if os.environ.get('QUERY_STRING') else ''}
            </div>
            
            <div class="env-grid" id="envGrid">"""
    
    # Add each environment variable
    for key, value in sorted_env_vars:
        # Escape HTML characters
        safe_key = html.escape(key)
        safe_value = html.escape(value) if value else '<em>empty</em>'
        
        html_content += f"""
                <div class="env-var" data-key="{safe_key.lower()}">
                    <div class="env-key">{safe_key}</div>
                    <div class="env-value">{safe_value}</div>
                </div>"""
    
    html_content += """
            </div>
            
            <div class="no-results" id="noResults" style="display: none;">
                <p>🔍 No environment variables found matching your search.</p>
            </div>
        </div>
    </main>

    <footer>
        <p>&copy; 2025 GreenLeaf Plant Catalog. Cultivating your server environment.</p>
    </footer>
    
    <script>
        // Search functionality
        const searchBox = document.getElementById('searchBox');
        
        // Trigger search on page load if there's a value
        if (searchBox.value) {
            performSearch(searchBox.value);
        }
        
        searchBox.addEventListener('input', function(e) {
            const searchTerm = e.target.value;
            performSearch(searchTerm);
            
            // Update URL with search parameter (without reloading page)
            const url = new URL(window.location);
            if (searchTerm) {
                url.searchParams.set('search', searchTerm);
            } else {
                url.searchParams.delete('search');
            }
            window.history.replaceState({}, '', url);
        });
        
        function performSearch(searchTerm) {
            const searchTermLower = searchTerm.toLowerCase();
            const envVars = document.querySelectorAll('.env-var');
            const noResults = document.getElementById('noResults');
            let visibleCount = 0;
            
            envVars.forEach(function(envVar) {
                const key = envVar.dataset.key;
                const value = envVar.querySelector('.env-value').textContent.toLowerCase();
                
                if (key.includes(searchTermLower) || value.includes(searchTermLower)) {
                    envVar.style.display = 'block';
                    visibleCount++;
                } else {
                    envVar.style.display = 'none';
                }
            });
            
            noResults.style.display = visibleCount === 0 ? 'block' : 'none';
        }
        
        // Add copy functionality to environment variables
        document.querySelectorAll('.env-var').forEach(function(envVar) {
            envVar.addEventListener('click', function() {
                const key = this.querySelector('.env-key').textContent;
                const value = this.querySelector('.env-value').textContent;
                const text = key + '=' + value;
                
                navigator.clipboard.writeText(text).then(function() {
                    // Visual feedback
                    const originalBorder = envVar.style.borderColor;
                    envVar.style.borderColor = '#32CD32';
                    envVar.style.backgroundColor = '#f0fff0';
                    
                    setTimeout(function() {
                        envVar.style.borderColor = originalBorder;
                        envVar.style.backgroundColor = '';
                    }, 1000);
                }).catch(function() {
                    console.log('Could not copy to clipboard');
                });
            });
        });
    </script>
</body>
</html>"""
    
    return html_content

def main():
    """Main CGI script function."""
    try:
        # Parse query parameters
        params = parse_query_params()
        
        # Get all environment variables
        env_vars = dict(os.environ)
        
        # Check if JSON format is requested
        if params.get('format', '').lower() == 'json':
            # Generate JSON response
            json_content = create_json_response(env_vars, params)
            
            # Print HTTP headers for JSON
            print("Content-Type: application/json; charset=utf-8", end="\r\n")
            print(f"Content-Length: {len(json_content.encode('utf-8'))}", end="\r\n")
            print("Status: 200 OK", end="\r\n")
            print("Access-Control-Allow-Origin: *", end="\r\n")  # Allow CORS
            print(end="\r\n")
            
            # Print JSON content
            print(json_content)
        else:
            # Generate HTML response
            html_content = create_html_page(params)
            
            # Print HTTP headers for HTML
            print("Content-Type: text/html; charset=utf-8", end="\r\n")
            print(f"Content-Length: {len(html_content.encode('utf-8'))}", end="\r\n")
            print("Status: 200 OK", end="\r\n")
            print(end="\r\n")
            
            # Print HTML content
            print(html_content)
        
    except Exception as e:
        # Error handling
        error_html = f"""<!DOCTYPE html>
<html>
<head>
    <title>Error - Environment Variables</title>
    <link rel="stylesheet" href="/static/style.css">
</head>
<body>
    <div class="container">
        <h1>🚨 Error</h1>
        <p>An error occurred while generating the environment variables page:</p>
        <pre>{html.escape(str(e))}</pre>
        <a href="/index.html">← Back to Home</a>
    </div>
</body>
</html>"""
        
        print("Content-Type: text/html; charset=utf-8", end="\r\n")
        print(f"Content-Length: {len(error_html.encode('utf-8'))}", end="\r\n")
        print("Status: 500 Internal Server Error", end="\r\n")
        print(end="\r\n")
        print(error_html)

if __name__ == "__main__":
    main()