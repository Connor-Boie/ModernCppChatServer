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

## Build

```bash
mkdir build
cd build

cmake ..
cmake --build .
```

## Roadmap

- [x] Project setup
- [x] Socket abstraction
- [x] Create TCP socket
- [x] Listen for connections
- [ ] Accept multiple clients
- [ ] Thread-per-client model
- [ ] Message broadcasting
- [ ] Logging
- [ ] Unit tests