# WebServ

A lightweight HTTP/1.1 server implementation written in C++98. This project demonstrates building a web server from scratch with support for static file serving, CGI execution, file uploads, and comprehensive HTTP protocol handling.

## Features

- **HTTP/1.1 Protocol Support** - Full implementation of HTTP/1.1 specifications
- **Static File Serving** - Serve HTML, CSS, JavaScript, and other static content
- **CGI Support** - Execute Python and Shell scripts via Common Gateway Interface
- **File Upload Handling** - Support for file uploads to specified locations
- **Custom Error Pages** - Configurable error pages for HTTP status codes (403, 404, 405, 500)
- **Multiple HTTP Methods** - Support for GET, POST, and DELETE methods
- **Autoindex** - Automatic directory listing when enabled
- **Redirections** - HTTP 302 redirects to other locations
- **Cookie Management** - Basic cookie handling capabilities
- **Configuration File** - Nginx-style configuration syntax
- **Multiple Location Blocks** - Configure different behaviors for different URL paths

## Project Structure

```
WebServ/
├── srcs/
│   ├── cgi/              # CGI execution modules
│   ├── config/           # Configuration parsing and management
│   ├── parsing/          # HTTP request/response parsing
│   ├── webserv/          # Core web server implementation
│   ├── ProjectTools.cpp  # Utility functions
│   └── main.cpp          # Entry point
├── conf/                 # Additional configuration files
├── var/www/              # Default document root
├── .vscode/              # VSCode configuration
├── simple.conf           # Example configuration file
├── Makefile              # Build instructions
└── .gitignore
```

## Prerequisites

- **C++ Compiler** - Supporting C++98 standard (g++, clang++)
- **Make** - For building the project
- **Python3** - For Python CGI scripts (optional)
- **Bash** - For Shell CGI scripts (optional)

## Building the Project

```bash
make
```

This will compile the source code and generate the `webserv` executable.

### Additional Make Commands

```bash
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Rebuild the project
```

## Usage

```bash
./webserv [configuration_file]
```

**Example:**
```bash
./webserv simple.conf
```

If no configuration file is specified, the server will look for a default configuration.

## Configuration

The server uses an Nginx-style configuration format. Here's an example from `simple.conf`:

```nginx
error_log logs/error.log;
client_max_body_size 1000000;

server {
    listen 8080;
    server_name localhost;
    root var/www;
    index index.html;
    
    error_page 403 /errors/403.html;
    error_page 404 /errors/404.html;
    error_page 405 /errors/405.html;
    error_page 500 /errors/500.html;
    
    location / {
        allow_methods GET;
    }
    
    location /uploads {
        root var/www;
        upload_path var/www;
        index index.html;
        allow_methods POST GET DELETE;
    }
    
    location /cgi {
        cgi ".py /usr/bin/python3";
        cgi ".sh /bin/bash";
        upload_path var/www;
        allow_methods POST GET;
    }
    
    location /directory {
        autoindex on;
    }
    
    location /redirection {
        return 302 "https://nginx.org/";
    }
}
```

### Configuration Directives

#### Global Directives
- `error_log` - Path to error log file
- `client_max_body_size` - Maximum size of client request body in bytes

#### Server Block Directives
- `listen` - Port number to listen on
- `server_name` - Server name(s)
- `root` - Document root directory
- `index` - Default index file
- `error_page` - Custom error page for specific HTTP status codes

#### Location Block Directives
- `allow_methods` - Allowed HTTP methods (GET, POST, DELETE)
- `root` - Override document root for this location
- `upload_path` - Directory for file uploads
- `autoindex` - Enable/disable directory listing (on/off)
- `cgi` - CGI interpreter configuration (file extension and path)
- `return` - HTTP redirect with status code and URL

## Testing

Once the server is running, you can test it using:

**Web Browser:**
```
http://localhost:8080/
```

**curl:**
```bash
# GET request
curl http://localhost:8080/

# POST request with file upload
curl -X POST -F "file=@example.txt" http://localhost:8080/uploads

# DELETE request
curl -X DELETE http://localhost:8080/uploads/example.txt
```

## Features in Detail

### CGI Support
The server can execute CGI scripts written in Python or Shell. Configure the CGI interpreter in your location block:

```nginx
location /cgi {
    cgi ".py /usr/bin/python3";
    cgi ".sh /bin/bash";
    allow_methods POST GET;
}
```

### File Uploads
Enable file uploads by specifying an `upload_path` in a location block:

```nginx
location /uploads {
    upload_path var/www/uploads;
    allow_methods POST;
}
```

### Directory Listing
Enable automatic directory listing with autoindex:

```nginx
location /directory {
    autoindex on;
}
```

### Custom Error Pages
Provide user-friendly error pages:

```nginx
error_page 404 /errors/404.html;
error_page 500 /errors/500.html;
```

## Development

The project is structured into modular components:

- **Config Module** - Parses and validates configuration files
- **Parsing Module** - Handles HTTP request and response parsing
- **WebServ Module** - Core server logic, socket management, and client handling
- **CGI Module** - Executes and manages CGI scripts

## Contributors

- [Keoma Ricci de Lima](https://github.com/keomalima)
- [Ly-Sha](https://github.com/lyshathan)

## Technical Details

- **Language:** C++98
- **Non-blocking I/O** using poll()
- **Socket Programming** for network communication
- **Multi-client Support** via multiplexing

## Known Limitations

- HTTP/1.1 only (no HTTP/2 or HTTP/3 support)
- Basic CGI implementation (not FastCGI)
- Single-threaded architecture

## License

This is an educational project developed as part of the 42 curriculum.

---

**Note:** This project is for educational purposes and is not intended for production use.
