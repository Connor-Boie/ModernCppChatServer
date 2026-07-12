# Modern C++ TCP Chat Server

A portfolio project demonstrating modern C++17, Linux/POSIX socket programming, RAII, move semantics, multithreading, synchronization, and object-oriented design.

## Goals

This project is being built incrementally to explore:

- Modern C++17 programming practices
- Linux/POSIX socket programming
- TCP/IP networking fundamentals
- Object-oriented design
- Resource management using RAII
- Move semantics and unique ownership
- Multithreading and synchronization
- CMake build systems
- Unit testing

## Concepts Demonstrated

- Modern C++17
- RAII resource management
- Move semantics and ownership transfer
- Deleted copy operations for unique resource ownership
- POSIX socket programming
- TCP/IP networking
- Thread-per-client concurrency with `std::thread`
- Thread synchronization with `std::mutex`
- RAII-based locking with `std::lock_guard`
- Separation of responsibilities using `Socket` and `Server` classes
- CMake build configuration
- Shared ownership using `std::shared_ptr`
- Thread-safe management of shared client connections

## Current Status

In progress. The server currently:

- Creates, binds, and listens on a TCP socket
- Accepts multiple client connections
- Handles each client on a detached worker thread
- Receives messages and sends acknowledgements
- Logs output with mutex protection
- Cleans up sockets automatically using RAII
- Broadcasts messages to other connected clients

## Implemented Features

### Networking

- TCP socket creation using Linux/POSIX APIs
- IPv4 binding to port `8080`
- Listening for incoming connections
- Accepting client connections
- Receiving client messages
- Sending server responses
- Detection of graceful client disconnects
- Handling of partial sends
- Broadcasting messages between connected clients
- Tracking active client connections

### Modern C++

- C++17
- RAII-based `Socket` ownership
- Move constructor and move assignment operator
- Deleted copy constructor and copy assignment operator
- Exception-based error handling
- Dedicated `Server` class for server lifecycle management

### Concurrency

- Thread-per-client architecture
- Independent handling of multiple clients
- Mutex-protected console output
- RAII locking with `std::lock_guard`
- Thread-safe connected-client storage
- Synchronized message broadcasting

### Build System

- CMake-based build configuration
- Out-of-source builds
- Compiler warnings enabled with `-Wall`, `-Wextra`, and `-Wpedantic`

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

## Run

Start the server from the project root:

```bash
./build/ModernCppChatServer
```

From another terminal, connect with netcat:

```bash
nc localhost 8080
```

Type a message and press Enter. The client should receive:

```text
Message received!
```

## Roadmap

- [x] Project setup
- [x] RAII socket abstraction
- [x] TCP socket creation
- [x] Bind and listen
- [x] Accept client connections
- [x] Receive and send data
- [x] Move-only socket ownership
- [x] Thread-per-client architecture
- [x] Mutex-protected logging
- [x] Encapsulate server lifecycle in a `Server` class
- [x] Track active clients
- [x] Broadcast messages between clients
- [ ] Add usernames
- [ ] Add graceful server shutdown
- [ ] Add structured logging
- [ ] Add unit tests