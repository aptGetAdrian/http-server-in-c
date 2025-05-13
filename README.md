# C HTTP Server

A lightweight, multi-threaded HTTP server implemented in C.

## Features

- **Multi-threaded design**: Handles multiple client connections simultaneously
- **HTTP protocol support**: Implements the basic HTTP GET and POST methods
- **Static file serving**: Serves various file types with appropriate MIME types
- **User agent endpoint**: Built-in `/user-agent` endpoint that returns the client's user agent
- **File upload**: Support for POST requests to upload files
- **Customizable status codes**: Reads status codes from an external file

## Technical description

### Supported File Types

The server can serve various file types including:
- HTML (.html)
- CSS (.css)
- JavaScript (.js)
- Images (.png, .jpg, .jpeg, .gif, .svg)
- JSON (.json)
- PDF (.pdf)

### Architecture

The server uses a thread-per-connection model:
- Main thread listens for incoming connections
- Each client connection is handled in a separate thread
- Threads are detached to avoid memory leaks

### Libraries used

- Standard C libraries
- POSIX threading (pthread)
- [Custom vector implementation ](https://github.com/Mashpoe/c-vector/tree/master) for dynamic arrays by [Mashpoe](https://github.com/Mashpoe)

## How to run it

I recommend using the GCC compiler, as that's the one I use and know for sure that it works. I also included a *makefile*

### Build Instructions

1. Clone the repository
   ```bash
   git clone https://github.com/aptGetAdrian/http-server-in-c.git
   cd http-server-in-c
   ```

2. Build the server. Either use the makefile:
   ```bash
   make
   ```
   Or compile it manually:
   ```bash
   gcc server.c libraries/vec.c -o server -lpthread
   ```

## Usage
Run the server
```
./server
```
The server will listen on port 4221 by default

### Example requests

#### GET request
```bash
curl http://localhost:4221/echo/hello
```
Returns "hello" with appropriate headers

#### User Agent request
```bash
curl http://localhost:4221/user-agent
```
Returns the User-Agent header from the request

#### Serving files
You can also prepare a web page with HTML, CSS and JavaScript and open the page in your browser with the following URL: `http://localhost:4221/index.html`
Serves the index.html file from the server directory

#### POST request (file upload)
```bash
curl -X POST -d "file content" http://localhost:4221/upload.txt
```
Creates a file named "upload.txt" in the files directory with the provided content

## Future plans for this project

The following features are planned for future implementation:
- Thread pool for better resource management
- HTTP response compression
- Improved error handling
- Configuration file support
