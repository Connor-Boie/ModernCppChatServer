# Modern C++ TCP Chat Server

[![C++ CI](https://github.com/Connor-Boie/ModernCppChatServer/actions/workflows/ci.yml/badge.svg)](https://github.com/Connor-Boie/ModernCppChatServer/actions/workflows/ci.yml)

A multithreaded TCP chat server built with C++17 and Linux/POSIX socket APIs.

The server supports multiple simultaneous clients, unique usernames, public chat, private messaging, command handling, structured logging, graceful shutdown, automated unit tests, end-to-end integration testing, and continuous integration.

## Project Highlights

- Modern C++17 resource management
- RAII-based socket ownership
- Move-only file descriptor wrapper
- Thread-per-client concurrency model
- Mutex-protected shared state
- Public and private message routing
- Graceful `SIGINT` shutdown
- GoogleTest unit tests
- Python end-to-end integration testing
- CMake and CTest integration
- GitHub Actions continuous integration
- Architecture and data-flow documentation

## Architecture

The server uses one listening socket and creates one worker thread for each connected client.

Each worker thread:

1. Registers a unique username.
2. Waits for incoming TCP data.
3. Processes commands or public messages.
4. Routes private messages by username.
5. Removes the client during disconnection.
6. Exits cleanly during server shutdown.

Shared client, username, and logging state is protected using mutexes.

For detailed component, synchronization, message-flow, and shutdown diagrams, see:

[Architecture and Data-Flow Documentation](docs/ARCHITECTURE.md)

## Features

### Networking

- TCP socket creation using Linux/POSIX APIs
- IPv4 binding on port `8080`
- `SO_REUSEADDR` support
- Listening for incoming connections
- Multiple simultaneous clients
- Partial-send handling
- Graceful disconnect detection
- `SIGPIPE` prevention using `MSG_NOSIGNAL`
- Socket shutdown for interrupting blocked receive operations

### Chat

- Unique username registration
- Duplicate username prevention
- Public-message broadcasting
- Join and leave notifications
- Private messaging by username
- Connected-user listing
- Sender confirmation messages
- Unknown-command handling
- Clean client disconnection

### Supported Commands

```text
/help
```

Displays the available commands.

```text
/users
```

Displays currently connected usernames.

```text
/msg <username> <message>
```

Sends a private message to one connected user.

```text
/quit
```

Disconnects from the server cleanly.

### Modern C++

- C++17
- RAII resource management
- Move constructor and move assignment operator
- Deleted copy operations
- `std::shared_ptr` for shared client lifetime management
- `std::thread` worker threads
- `std::mutex` and `std::lock_guard`
- `std::unordered_set` for unique usernames
- `std::unordered_map` for username-to-socket lookup
- Exception-based error handling
- Strongly typed logging levels with `enum class`
- Independent utility functions for testable logic

### Logging

Server activity is logged with timestamps and severity levels.

Example:

```text
[2026-07-16 19:30:12] [INFO] Server listening on port 8080.
[2026-07-16 19:30:18] [INFO] Connor joined the chat. File descriptor: 4
[2026-07-16 19:31:07] [WARNING] Broadcast to client 5 failed: Broken pipe
```

Log output is protected by a mutex so messages from multiple worker threads do not become interleaved.

Output streams are explicitly flushed so startup and shutdown logs remain visible when the server is run through automated tests.

### Graceful Shutdown

Pressing `Ctrl+C` sends `SIGINT` to the server.

The server then:

1. Sets a shutdown-requested flag.
2. Exits the blocking accept loop.
3. Stops accepting new clients.
4. Shuts down active client sockets.
5. Wakes worker threads blocked in `recv()`.
6. Waits for all worker threads using `join()`.
7. Releases remaining socket resources through RAII.
8. Exits cleanly.

## Project Structure

```text
ModernCppChatServer/
├── .github/
│   └── workflows/
│       └── ci.yml
├── docs/
│   └── ARCHITECTURE.md
├── include/
│   ├── Server.h
│   ├── Socket.h
│   └── TextUtils.h
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── Socket.cpp
│   └── TextUtils.cpp
├── tests/
│   ├── integration_test.py
│   └── TextUtilsTests.cpp
├── CMakeLists.txt
└── README.md
```

## Requirements

- Linux or Windows Subsystem for Linux
- C++17-compatible compiler
- CMake 3.16 or newer
- POSIX thread support
- Python 3
- Git
- Internet access during the first CMake configuration so `FetchContent` can retrieve GoogleTest
- Netcat for optional manual testing

On Ubuntu or WSL:

```bash
sudo apt update
sudo apt install build-essential cmake git python3 netcat-openbsd
```

## Configure

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

The first configuration downloads the pinned GoogleTest dependency.

## Build

```bash
cmake --build build --parallel
```

This builds:

```text
ModernCppChatServer
TextUtilsTests
```

## Run the Server

```bash
./build/ModernCppChatServer
```

The server listens on:

```text
127.0.0.1:8080
```

## Connect Manually

Open another terminal:

```bash
nc localhost 8080
```

Open additional terminals with the same command to connect multiple clients.

Example:

```text
Enter your username: Connor
Welcome, Connor!
Type /help to view available commands.
```

## Example Public Chat

Connor sends:

```text
Hello everyone
```

Other clients receive:

```text
Connor: Hello everyone
```

Connor receives:

```text
Message sent.
```

## Example Private Message

Connor sends:

```text
/msg Alice Secret message
```

Alice receives:

```text
[Private from Connor] Secret message
```

Connor receives:

```text
[Private to Alice] Secret message
```

## Testing

The project uses two testing levels.

### C++ Unit Tests

GoogleTest verifies isolated C++ utility logic.

Run the unit-test executable directly:

```bash
./build/TextUtilsTests
```

Run only the discovered line-ending tests through CTest:

```bash
ctest \
    --test-dir build \
    -R RemoveLineEndingTest \
    --output-on-failure
```

### Python Integration Test

The Python integration test treats the server as an external process.

It verifies:

- Server startup
- TCP client connections
- Username registration
- Public-message broadcasting
- `/users`
- Private messaging
- `/quit`
- Graceful `SIGINT` shutdown
- Clean process termination

Run it directly:

```bash
python3 \
    tests/integration_test.py \
    ./build/ModernCppChatServer
```

### Complete Test Suite

Run all C++ and Python tests:

```bash
ctest --test-dir build --output-on-failure
```

For verbose output:

```bash
ctest --test-dir build -V
```

Make sure another server instance is not already using port `8080` before running the integration test.

Check the port with:

```bash
ss -ltnp | grep :8080
```

## Continuous Integration

GitHub Actions automatically configures, builds, and tests the project when:

- A commit is pushed to `main`
- A pull request targets `main`
- The workflow is run manually

The workflow performs:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The build status is displayed by the badge at the top of this README.

## Design Decisions

### RAII Socket Ownership

Each `Socket` object owns exactly one file descriptor and closes it during destruction.

Copy operations are disabled because copying a raw file descriptor would create unclear ownership and could result in closing the same descriptor more than once.

Move operations transfer ownership safely.

### Thread Per Client

Each connected client receives a dedicated worker thread.

This design provides straightforward blocking control flow and makes the relationship between a client and its worker easy to understand.

A large production server might instead use:

- A thread pool
- Nonblocking sockets
- `select`
- `poll`
- `epoll`
- An event-driven architecture

### Shared Client Ownership

Client sockets use `std::shared_ptr` because a socket can temporarily be referenced by:

- The active-client collection
- Its worker thread
- The username lookup map
- A broadcast operation
- A private-message operation
- The shutdown process

The underlying file descriptor is still owned by one `Socket` object.

### Separate Testing Tools

GoogleTest is used for isolated C++ behavior.

Python is used for process orchestration and end-to-end TCP testing.

CTest provides one command for running both test types.

## Current Limitations

- The server supports Linux/POSIX environments rather than Windows sockets.
- Messages are newline-delimited rather than length-prefixed.
- The server uses one operating-system thread per client.
- The listening port is currently fixed at `8080`.
- Chat messages are not encrypted.
- Users are not authenticated.
- Connected-client storage may retain completed joinable thread objects until server shutdown.
- The project is intended as a systems-programming portfolio project rather than a production chat service.

## Potential Future Improvements

- Configurable address and port
- Length-prefixed message framing
- Additional command-parser unit tests
- Stress and concurrency testing
- AddressSanitizer and UndefinedBehaviorSanitizer builds
- Multiple compiler configurations in CI
- Per-client outgoing-message queues
- Thread-pool or event-driven architecture
- Authentication
- TLS encryption

## Skills Demonstrated

- Modern C++ development
- Linux systems programming
- TCP/IP socket programming
- Object-oriented design
- Resource ownership and RAII
- Move semantics
- Multithreading
- Synchronization
- Shared-state management
- Signal handling
- CMake
- GoogleTest
- CTest
- Python test automation
- GitHub Actions
- Technical documentation
- Architecture communication

## Roadmap

- [x] Project setup
- [x] RAII socket abstraction
- [x] TCP socket creation
- [x] Bind and listen
- [x] Accept client connections
- [x] Receive and send data
- [x] Handle partial sends
- [x] Move-only socket ownership
- [x] Thread-per-client architecture
- [x] Mutex-protected logging
- [x] Server lifecycle encapsulation
- [x] Active-client tracking
- [x] Public-message broadcasting
- [x] Username registration
- [x] Duplicate username prevention
- [x] Chat commands
- [x] Private messaging
- [x] Timestamped structured logging
- [x] Graceful `Ctrl+C` handling
- [x] Worker-thread shutdown and joining
- [x] Python integration tests
- [x] GoogleTest C++ unit tests
- [x] GitHub Actions continuous integration
- [x] Architecture and data-flow documentation
- [x] Portfolio documentation

## License

This project is available for educational and portfolio purposes.