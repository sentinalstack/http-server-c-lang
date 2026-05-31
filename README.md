# HTTP Server in C

## Overview

This project is a minimal HTTP/1.1 web server implemented entirely in C using POSIX sockets.

The server listens on TCP port `8080`, accepts incoming client connections, parses basic HTTP requests, and responds to HTTP `GET` requests with a simple plaintext response.

The primary goal of this project is educational: understanding how HTTP communication works at the socket level without relying on external frameworks or libraries.

---

## Features

* Raw TCP socket programming
* IPv4 networking support
* HTTP/1.1 response generation
* Request handling using `recv()`
* Multiple sequential client connections
* Manual implementation of reliable data transmission with `send_all()`
* Reusable socket address resolution using `getaddrinfo()`
* Port reuse using `SO_REUSEADDR`

---

## Project Structure

```text
main.c
```

All server logic is implemented inside a single source file.

---

## Build

### Linux

```bash
gcc main.c -o server
```

### Run

```bash
./server
```

Expected output:

```text
Listening on :8080
```

---

## Testing

### Browser

Open:

```text
http://localhost:8080
```

Response:

```text
Hello, world!
```

---

### Using curl

```bash
curl http://localhost:8080
```

Output:

```text
Hello, world!
```

---

## HTTP Request Flow

When a client sends:

```http
GET / HTTP/1.1
Host: localhost:8080
```

The server:

1. Accepts the TCP connection.
2. Reads request data into a buffer.
3. Checks whether the request begins with `"GET"`.
4. Constructs an HTTP response.
5. Sends the response.
6. Closes the client connection.

---

## Core Concepts

### 1. Socket Creation

```c
socket()
```

Creates an endpoint for network communication.

The socket returned by the kernel represents a file descriptor that can be used for reading and writing network data.

```text
Application
     |
 Socket FD
     |
 Operating System
     |
   Network
```

---

### 2. Address Resolution

```c
getaddrinfo()
```

Converts human-readable configuration into structures usable by the networking API.

Configuration:

```c
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = IPPROTO_TCP;
```

Meaning:

| Setting     | Purpose               |
| ----------- | --------------------- |
| AF_INET     | IPv4                  |
| SOCK_STREAM | TCP communication     |
| IPPROTO_TCP | Explicit TCP protocol |

---

### 3. Binding

```c
bind()
```

Associates the socket with a local address and port.

```text
0.0.0.0:8080
```

This allows the operating system to route incoming packets for port `8080` to the server process.

---

### 4. Listening

```c
listen(fd, 20);
```

Converts the socket into a passive listening socket.

The kernel maintains a connection queue:

```text
Incoming Clients
       |
       v
+----------------+
| Listen Queue   |
+----------------+
       |
       v
    accept()
```

The value `20` specifies the maximum backlog queue size.

---

### 5. Accepting Connections

```c
accept()
```

Accepts a pending client connection.

Important:

* The listening socket remains open.
* A new socket is returned for communication with the client.

```text
Listening Socket
       |
       +---- Client A Socket
       |
       +---- Client B Socket
       |
       +---- Client C Socket
```

Each accepted connection receives its own file descriptor.

---

### 6. Receiving Data

```c
recv()
```

Reads bytes from the client socket.

Example request:

```http
GET / HTTP/1.1
Host: localhost:8080
User-Agent: curl
```

The server stores this request inside:

```c
char buf[MAX_LEN];
```

---

### 7. Request Detection

Current implementation:

```c
strncmp(buf, "GET", 3) == 0
```

Only the request method is inspected.

Supported:

```http
GET
```

Not supported:

```http
POST
PUT
DELETE
PATCH
HEAD
OPTIONS
```

A production HTTP server would parse:

* Method
* Path
* Headers
* HTTP Version
* Body

---

### 8. HTTP Response Construction

The response is manually written as a string:

```http
HTTP/1.1 200 OK
Content-Length: 13
Content-Type: text/plain

Hello, world!
```

HTTP responses consist of:

```text
Status Line
Headers
Blank Line
Body
```

Example:

```http
HTTP/1.1 200 OK
Content-Length: 13
Content-Type: text/plain

Hello, world!
```

---

### 9. Reliable Sending

The server uses:

```c
send_all()
```

instead of a single call to:

```c
send()
```

Reason:

`send()` is not guaranteed to transmit all bytes in one operation.

Example:

```text
Requested: 1000 bytes
Actually sent: 400 bytes
```

The remaining bytes must be transmitted manually.

The helper function repeatedly calls `send()` until the entire response has been delivered.

Pseudo flow:

```text
while(bytes_remaining > 0)
    send()
```

This is an important concept in network programming because TCP provides a byte stream rather than message boundaries.

---

### 10. Connection Termination

After sending the response:

```c
close(new_fd);
```

The client connection is terminated.

The listening socket remains active and continues accepting new connections.

---

## Server Lifecycle

```text
socket()
   |
getaddrinfo()
   |
bind()
   |
listen()
   |
accept()
   |
recv()
   |
process request
   |
send()
   |
close(client)
   |
repeat
```

---

## Limitations

This server intentionally remains minimal.

Current limitations include:

* IPv4 only
* Single-threaded
* No concurrency
* No HTTP request parsing beyond method detection
* No routing
* No static file serving
* No MIME type detection
* No persistent connections
* No HTTPS/TLS
* No chunked transfer encoding
* No logging system
* No error response generation
* No support for POST request bodies

---

## Potential Improvements

### Routing

```text
/
/about
/api/users
/api/products
```

---

### Static File Serving

Serve:

```text
HTML
CSS
JavaScript
Images
```

---

### Thread-per-Connection Model

```text
Client A -> Thread A
Client B -> Thread B
Client C -> Thread C
```

---

### Event-Driven Architecture

Using:

```text
select()
poll()
epoll()
kqueue()
```

to handle thousands of simultaneous connections.

---

### HTTP Parser

Implement parsing for:

```http
Method
Path
Version
Headers
Body
```

---

### HTTPS Support

Add TLS support through libraries such as:

```text
OpenSSL
LibreSSL
BoringSSL
```

---


## References

* RFC 9112 — HTTP/1.1
* RFC 9293 — TCP
* POSIX Socket API Documentation
* Beej's Guide to Network Programming
