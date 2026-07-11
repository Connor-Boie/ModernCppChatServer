# ModernCppChatServer
A multithreaded TCP chat server built with modern C++17, CMake, and Linux sockets.

## Goals

This project is being built incrementally to explore:

- Modern C++17
- Linux TCP sockets
- Object-oriented design
- Resource management with RAII
- Multithreading
- Synchronization with mutexes
- CMake
- Unit testing

## Current Status

In Progress

Current milestone:

- Project structure
- RAII-based Socket abstraction
- TCP socket creation using Linux socket APIs
- Socket binding to an IP address and port
- Socket listening for incoming client connections
- Accepting client connections
- Receiving a message from the client
- Sending a message to the connected client.

## Implemented Features

### Networking

- RAII-based `Socket` class for automatic socket resource management
- TCP socket creation using the Linux/POSIX socket API
- IPv4 socket binding to configurable ports
- Listening for incoming TCP client connections
- Accepting client connections
- Receiving data from connected clients
- Sending responses to connected clients

### Modern C++

- C++17
- RAII for resource management
- Move semantics for safe ownership transfer
- Deleted copy operations for unique resource ownership
- Exception-based error handling
- Encapsulation through an object-oriented `Socket` abstraction

### Build System

- CMake-based build configuration
- C++17 standard enabled
- Compiler warnings enabled (`-Wall`, `-Wextra`, `-Wpedantic`)

## Build

```bash
mkdir build
cd build

cmake ..
cmake --build .
```

## Run

Start the server:

```bash
./ModernCppChatServer
```

The server will start listening:

```
Server listening on port 8080
```

Connect using netcat:

```bash
nc localhost 8080
```

Send a message:

```bash
hello world
```

The server will report the received message from the client:

```
Client connected!
Received: hello world
```

## Roadmap

- [x] Project setup
- [x] Socket abstraction
- [x] Create TCP socket
- [x] Bind socket to address and port
- [x] Listen for incoming connections
- [x] Accept client connections
- [x] Receive data from clients
- [x] Send responses to clients
- [x] Implement move semantics for socket ownership
- [ ] Support multiple simultaneous clients
- [ ] Thread-per-client architecture
- [ ] Synchronization with mutexes
- [ ] Broadcast messages to all clients
- [ ] Logging system
- [ ] Unit tests