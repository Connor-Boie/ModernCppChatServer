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
- Command parsing and control flow
- Building formatted strings with `std::ostringstream`
- Associating usernames with client sockets using `std::unordered_map`
- Parsing commands with `std::istringstream`
- Safely copying shared resources before releasing a mutex
- Strongly typed enumerations using `enum class`
- Date and time handling with `std::chrono`
- Time formatting with `std::put_time`
- Selecting output streams through `std::ostream`
- Linux signal handling with `sigaction`
- Signal-safe communication using `std::sig_atomic_t`
- Interrupting blocking system calls
- Graceful control-flow termination
- Managing thread lifetimes with `std::thread::join`
- Interrupting blocked socket reads with `shutdown`
- Coordinated multithreaded shutdown
- Copying shared resources before releasing a mutex

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
- Supports `/help`, `/users`, and `/quit` chat commands
- Supports private messages between connected users
- Produces timestamped server logs with informational, warning, and error levels
- Handles `Ctrl+C` by exiting the server accept loop cleanly
- Shuts down connected client sockets and waits for worker threads before exiting

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
- `/help` command for command documentation
- `/users` command for viewing connected usernames
- `/quit` command for clean client disconnection
- Unknown-command feedback
- Private messaging with `/msg <username> <message>`
- Confirmation messages for private-message senders
- Feedback when a private-message recipient is unavailable

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

### Logging

- Timestamped server log messages
- Informational, warning, and error log levels
- Thread-safe console output
- Error messages written to standard error

### Shutdown

- `SIGINT` handling for `Ctrl+C`
- Clean exit from the blocking server accept loop
- RAII cleanup of the listening socket
- Coordinated shutdown of connected client sockets
- Worker-thread tracking and joining
- Safe server destruction after client threads finish

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
- [x] Add chat commands
- [x] Add private messaging
- [x] Add structured logging
- [x] Add graceful server shutdown
- [x] Stop and join worker threads during shutdown
- [ ] Add unit tests
- [ ] Add integration tests
- [ ] Finalize portfolio documentation