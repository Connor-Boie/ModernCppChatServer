## Goals

This project is being built incrementally to explore:

- Modern C++17 programming practices
- Linux/POSIX socket programming
- TCP/IP networking fundamentals
- Object-oriented design
- Resource management using RAII
- Move semantics and ownership models
- Multithreading and synchronization
- CMake build systems
- Unit testing

## Concepts Demonstrated

- Modern C++17
- Object-oriented design
- RAII resource management
- Move semantics and ownership transfer
- Deleted copy operations for unique resource ownership
- POSIX socket programming
- TCP/IP networking
- Multithreading with `std::thread`
- Thread synchronization with `std::mutex`
- RAII-based locking with `std::lock_guard`
- CMake build system

## Current Status

In Progress

Current milestone:

- TCP server capable of accepting client connections
- Client communication using send/receive operations
- Multiple clients handled concurrently using threads
- Thread-safe logging using mutex protection

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
- Encapsulation through an object-oriented `Socket` abstraction
- RAII for automatic resource cleanup
- Move constructor and move assignment for safe socket ownership transfer
- Deleted copy constructor and copy assignment operator to prevent invalid resource duplication
- Exception-based error handling

### Concurrency

- Thread-per-client architecture using `std::thread`
- Independent client handling
- Mutex protection for shared output resources
- RAII-based locking using `std::lock_guard`

### Build System

- CMake-based build configuration
- Out-of-source builds
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

Connect using netcat:

```bash
nc localhost 8080
```

Example:

Server:
```
Server listening on port 8080
Client connected
Received: hello world
```

Client:
```
hello world
Message received!
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
- [x] Thread-per-client architecture
- [x] Synchronization with mutexes
- [ ] Client connection manager
- [ ] Broadcast messages between clients
- [ ] Graceful client disconnect handling
- [ ] Logging system
- [ ] Unit tests