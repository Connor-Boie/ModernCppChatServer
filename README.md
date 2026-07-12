# Modern C++ TCP Chat Server

A portfolio project demonstrating modern C++17, Linux/POSIX socket programming, RAII, move semantics, multithreading, synchronization, shared ownership, and object-oriented design.

## Goals

This project is being built incrementally to explore:

- Modern C++17 programming practices
- Linux/POSIX socket programming
- TCP/IP networking fundamentals
- Object-oriented design
- Resource management using RAII
- Move semantics and unique ownership
- Shared ownership using smart pointers
- Multithreading and synchronization
- CMake build systems
- Unit testing

## Concepts Demonstrated

- Modern C++17
- RAII resource management
- Move semantics and ownership transfer
- Deleted copy operations for unique resource ownership
- Shared ownership using `std::shared_ptr`
- POSIX socket programming
- TCP/IP networking
- Thread-per-client concurrency with `std::thread`
- Thread synchronization with `std::mutex`
- RAII-based locking with `std::lock_guard`
- Thread-safe management of shared client connections
- Separation of responsibilities using `Socket` and `Server` classes
- CMake build configuration
- Unique-value storage using `std::unordered_set`
- Atomic check-and-insert operations protected by a mutex

## Current Status

In progress. The server currently:

- Creates, binds, and listens on a TCP socket
- Accepts multiple client connections
- Handles each client on a detached worker thread
- Prompts connected clients for a username
- Receives messages from connected clients
- Broadcasts messages to other connected clients
- Announces when users join or leave
- Logs output with mutex protection
- Cleans up sockets automatically using RAII
- Prevents multiple connected clients from using the same username

## Implemented Features

### Networking

- TCP socket creation using Linux/POSIX APIs
- IPv4 binding to port `8080`
- Socket address reuse for quick server restarts
- Listening for incoming connections
- Accepting client connections
- Receiving client messages
- Sending server responses
- Detection of graceful client disconnects
- Handling of partial sends
- Broadcasting messages between connected clients
- Tracking active client connections
- Duplicate username prevention
- Username release when a client disconnects

### Chat

- Username selection when connecting
- Username-based chat messages
- Join notifications
- Leave notifications
- Automatic guest username for an empty username

### Modern C++

- C++17
- RAII-based `Socket` ownership
- Move constructor and move assignment operator
- Deleted copy constructor and copy assignment operator
- Shared client ownership using `std::shared_ptr`
- Exception-based error handling
- Dedicated `Server` class for server lifecycle management
- Internal helper functions using an anonymous namespace

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

Configure the project:

```bash
cmake -S . -B build
```

Compile the project:

```bash
cmake --build build
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

The server asks for a username:

```text
Enter your username:
```

After entering a username, type messages to send them to the other connected clients.

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
- [x] Add usernames
- [x] Prevent duplicate usernames
- [ ] Add chat commands
- [ ] Add graceful server shutdown
- [ ] Add structured logging
- [ ] Add unit tests