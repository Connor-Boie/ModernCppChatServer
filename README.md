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

## Implemented Features

### Networking

- Created a RAII-based C++ Socket wrapper
- Created TCP sockets using Linux socket APIs
- Bound sockets to IPv4 addresses and ports
- Configured sockets to listen for incoming connections

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

```bash
./ModernCppChatServer
```

Current output:

```
Server listening on port 8080.
```

## Roadmap

- [x] Project setup
- [x] Socket abstraction
- [x] Create TCP socket
- [x] Bind socket to address and port
- [x] Listen for incoming connections
- [ ] Accept client connections
- [ ] Client connection management
- [ ] Receive and send messages
- [ ] Multiple client support
- [ ] Thread-per-client model
- [ ] Thread pool architecture
- [ ] Synchronization with mutexes
- [ ] Logging system
- [ ] Unit tests