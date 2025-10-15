# 🌐 Webserv — Poll Loop Structure and Client Handling

This document explains how to handle all file descriptors (server sockets, clients, and CGI pipes)
using **a single `poll()` loop**, as required by the Webserv subject.

---

## 🧩 Data Structures

```cpp
std::vector<struct pollfd> _pollFds;
std::map<int, Client*> _clients;
```

Each `pollfd` tracks one file descriptor:

```cpp
struct pollfd {
    int fd;        // File descriptor (socket, pipe, etc.)
    short events;  // What to monitor (POLLIN, POLLOUT, etc.)
    short revents; // What happened (set by poll())
};
```

---

## ⚙️ Initialization

Add all listening sockets at startup:

```cpp
for (each server socket) {
    struct pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN; // Wait for new connections
    pfd.revents = 0;
    _pollFds.push_back(pfd);
}
```

---

## 🧠 Main Poll Loop

```cpp
int Webserv::runPollLoop() {
    while (true) {
        int ret = poll(_pollFds.data(), _pollFds.size(), POLL_TIMEOUT);
        if (ret < 0)
            throw std::runtime_error("poll() failed");
        if (ret == 0)
            handleTimeouts(); // optional

        for (size_t i = 0; i < _pollFds.size(); ++i) {
            struct pollfd &pfd = _pollFds[i];  // reference to the entry

            if (pfd.revents & POLLIN)
                handleReadEvent(pfd);
            else if (pfd.revents & POLLOUT)
                handleWriteEvent(pfd);
            else if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
                disconnectClient(pfd.fd);
        }
    }
}
```

---

## 👋 Accepting New Clients

When `poll()` signals activity on a listening socket:

```cpp
void Webserv::handleReadEvent(struct pollfd &pfd) {
    if (pfd.fd == _serverSocketFd) {
        acceptNewConnection(); // calls addClient()
        return;
    }

    Client *client = _clients[pfd.fd];
    int ret = client->readRequest();

    if (ret == READ_COMPLETE) {
        client->parseRequest();

        if (client->isCgi()) {
            startCgi(client);
            pfd.events = POLLOUT; // write to CGI stdin
        } else {
            client->prepareResponse();
            pfd.events = POLLOUT; // ready to send response
        }
    } else if (ret == READ_ERROR) {
        disconnectClient(pfd.fd);
    }
}
```

---

## 🧩 Adding a New Client

```cpp
void Webserv::addClient(int newClientFd, const std::string &clientIP) {
    _clients[newClientFd] = new Client(newClientFd, _config, clientIP);

    struct pollfd newClientPollFd;
    newClientPollFd.fd = newClientFd;
    newClientPollFd.events = POLLIN; // ready to read request
    newClientPollFd.revents = 0;

    _pollFds.push_back(newClientPollFd);
}
```

✅ This line is essential:
```cpp
_pollFds.push_back(newClientPollFd);
```
Without it, `poll()` would never monitor the new client socket.

---

## ✉️ Writing the Response

```cpp
void Webserv::handleWriteEvent(struct pollfd &pfd) {
    Client *client = _clients[pfd.fd];
    int ret = client->writeResponse();

    if (ret == WRITE_COMPLETE) {
        if (client->connectionShouldClose()) {
            disconnectClient(pfd.fd);
        } else {
            client->resetForNextRequest();
            pfd.events = POLLIN; // back to reading mode
        }
    } else if (ret == WRITE_ERROR) {
        disconnectClient(pfd.fd);
    }
}
```

---

## 🧠 Connection Persistence (Keep-Alive)

```cpp
bool Client::connectionShouldClose() const {
    return (httpVersion == "HTTP/1.0" && !keepAlive)
        || (headers.find("Connection") != headers.end()
            && headers.at("Connection") == "close");
}
```

| Situation | Close connection? |
|------------|-------------------|
| HTTP/1.0 without keep-alive | ✅ Yes |
| HTTP/1.1 default (keep-alive) | ❌ No |
| “Connection: close” | ✅ Yes |
| Timeout / error | ✅ Yes |

---

## 🧱 CGI Integration (simplified)

When launching a CGI:
```cpp
if (client->cgiState.state == CgiState::WRITING)
    addPollFd(client->cgiState.stdin_fd, POLLOUT);
else if (client->cgiState.state == CgiState::READING)
    addPollFd(client->cgiState.stdout_fd, POLLIN);
```

Handle these extra FDs inside the same poll loop.

---

## 🧹 Removing FDs Safely

When a client disconnects or you’re done:

```cpp
void Webserv::disconnectClient(int fd) {
    close(fd);
    delete _clients[fd];
    _clients.erase(fd);

    for (std::vector<struct pollfd>::iterator it = _pollFds.begin();
         it != _pollFds.end(); ++it)
    {
        if (it->fd == fd) {
            _pollFds.erase(it);
            break;
        }
    }
}
```

---

## 🔁 Summary of Event Flow

| State | `poll` events |
|--------|----------------|
| Waiting for request | `POLLIN` |
| Reading request body | `POLLIN` |
| Sending response | `POLLOUT` |
| CGI writing to stdin | `POLLOUT` |
| CGI reading from stdout | `POLLIN` |
| Error / disconnect | Remove FD |
| Keep-alive ready | Switch back to `POLLIN` |

---

## 🧠 Key Takeaways

- Use **one global `poll()`** for all sockets.
- Always add new FDs with `_pollFds.push_back()`.
- Switch `events` between `POLLIN` ↔ `POLLOUT` as client state changes.
- Never block — everything must be non-blocking.
- Remove or close FDs cleanly when finished or errored.
