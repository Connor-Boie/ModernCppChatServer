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
- Initial `Socket` abstraction

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
- [ ] Create TCP socket
- [ ] Listen for connections
- [ ] Accept multiple clients
- [ ] Thread-per-client model
- [ ] Message broadcasting
- [ ] Logging
- [ ] Unit tests