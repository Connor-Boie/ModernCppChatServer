# Modern C++ TCP Chat Server

A multithreaded TCP chat server built with modern C++17 and Linux/POSIX socket APIs.

The project demonstrates socket programming, RAII, move semantics, smart pointers, multithreading, synchronization, command parsing, graceful shutdown, structured logging, and automated integration testing.

## Goals

This project is being built incrementally to explore:

* Modern C++17 programming practices
* Linux/POSIX socket programming
* TCP/IP networking fundamentals
* Object-oriented design
* Resource management using RAII
* Move semantics and unique ownership
* Shared ownership using smart pointers
* Multithreading and synchronization
* Thread-safe shared state
* Command parsing
* Signal handling
* Automated testing
* CMake build systems

## Concepts Demonstrated

* Modern C++17
* RAII resource management
* Move semantics and ownership transfer
* Deleted copy operations for unique resource ownership
* Shared ownership using `std::shared_ptr`
* POSIX socket programming
* TCP/IP networking
* Thread-per-client concurrency with `std::thread`
* Thread synchronization with `std::mutex`
* RAII-based locking with `std::lock_guard`
* Thread-safe management of shared client connections
* Separation of responsibilities using `Socket` and `Server` classes
* Unique-value storage using `std::unordered_set`
* Associating usernames with client sockets using `std::unordered_map`
* Atomic check-and-insert operations protected by a mutex
* Command parsing and control flow
* Parsing command arguments with `std::istringstream`
* Building formatted strings with `std::ostringstream`
* Safely copying shared resources before releasing a mutex
* Strongly typed enumerations using `enum class`
* Date and time handling with `std::chrono`
* Time formatting with `std::put_time`
* Selecting output streams through `std::ostream`
* Linux signal handling with `sigaction`
* Signal-safe communication using `std::sig_atomic_t`
* Interrupting blocking system calls
* Graceful control-flow termination
* Managing thread lifetimes with `std::thread::join`
* Interrupting blocked socket reads with `shutdown`
* Coordinated multithreaded shutdown
* End-to-end integration testing
* Automated process management
* Programmatic TCP clients
* Test timeouts and cleanup
* CMake test registration with CTest
* C++ unit testing with GoogleTest
* Arrange–Act–Assert test structure
* Extracting pure logic into independently testable utilities
* Automatic GoogleTest discovery through CMake and CTest
* Dependency management using CMake `FetchContent`

## Current Status

The server currently:

* Creates, binds, and listens on a TCP socket
* Accepts multiple simultaneous client connections
* Handles each client on a worker thread
* Tracks connected client sockets
* Prompts clients to choose usernames
* Prevents duplicate active usernames
* Receives and broadcasts public messages
* Supports private messages between connected users
* Announces when users join or leave
* Supports `/help`, `/users`, `/msg`, and `/quit`
* Produces timestamped logs with informational, warning, and error levels
* Handles `Ctrl+C` using `SIGINT`
* Stops connected client sockets during shutdown
* Waits for all worker threads before exiting
* Cleans up sockets automatically using RAII
* Includes an automated end-to-end integration test

## Implemented Features

### Networking

* TCP socket creation using Linux/POSIX APIs
* IPv4 binding to port `8080`
* Socket address reuse using `SO_REUSEADDR`
* Listening for incoming TCP connections
* Accepting multiple client connections
* Receiving client messages
* Sending server responses
* Detection of graceful client disconnects
* Handling of partial sends
* Prevention of `SIGPIPE` using `MSG_NOSIGNAL`
* Broadcasting messages between connected clients
* Tracking active client connections
* Interrupting blocked socket operations during shutdown

### Chat

* Username selection when connecting
* Duplicate username prevention
* Username release when a client disconnects
* Username-based public chat messages
* Join notifications
* Leave notifications
* Private messaging using:

```text
/msg <username> <message>
```

* Confirmation messages for private-message senders
* Feedback when a private-message recipient is unavailable
* Unknown-command feedback

### Commands

The following chat commands are supported:

```text
/help
```

Displays the available commands.

```text
/users
```

Displays the usernames of currently connected users.

```text
/msg <username> <message>
```

Sends a private message to one connected user.

```text
/quit
```

Disconnects from the chat cleanly.

### Modern C++

* C++17
* RAII-based `Socket` ownership
* Move constructor and move assignment operator
* Deleted copy constructor and copy assignment operator
* Shared client ownership using `std::shared_ptr`
* Exception-based error handling
* Dedicated `Server` class for server lifecycle management
* Internal helper functions using anonymous namespaces
* `[[nodiscard]]` annotations
* `noexcept` move and cleanup operations
* `std::unordered_set` for unique usernames
* `std::unordered_map` for username-to-socket lookup
* `std::ostringstream` and `std::istringstream`
* Strongly typed logging levels using `enum class`

### Concurrency

* Thread-per-client architecture
* Independent handling of multiple clients
* Worker-thread storage using `std::vector<std::thread>`
* Worker-thread cleanup using `join()`
* Mutex-protected console output
* Thread-safe connected-client storage
* Thread-safe username registration
* Thread-safe username-to-socket lookup
* RAII locking with `std::lock_guard`
* Coordinated shutdown of blocked worker threads
* Safe server destruction after all workers finish

### Logging

* Timestamped server log messages
* Informational, warning, and error log levels
* Thread-safe console output
* Error messages written to standard error
* Explicit stream flushing for redirected output and automated tests

Example output:

```text
[2026-07-13 20:14:32] [INFO] Server listening on port 8080.
[2026-07-13 20:14:37] [INFO] Connor joined the chat. File descriptor: 4
[2026-07-13 20:15:02] [WARNING] Broadcast to client 5 failed: Broken pipe
```

### Shutdown

* `SIGINT` handling for `Ctrl+C`
* Clean exit from the blocking server accept loop
* Signal-safe shutdown request flag
* Shutdown of connected client sockets
* Interruption of blocked `recv()` calls
* Worker-thread tracking and joining
* RAII cleanup of socket file descriptors
* Safe server destruction after client threads finish

### Testing

* Automated server startup
* Automated TCP client connections
* Username-registration verification
* Public-message broadcast verification
* Connected-user command verification
* Private-message verification
* Clean-disconnection verification
* Graceful shutdown verification
* Test timeouts
* Automatic cleanup after failures
* CTest integration
* GoogleTest-based C++ unit tests
* Unit coverage for line-ending cleanup
* Automatic discovery of individual C++ test cases
* Unified C++ and Python test execution through CTest

## Project Structure

```text
ModernCppChatServer/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── Server.h
│   ├── Socket.h
│   └── TextUtils.h
├── src/
│   ├── Server.cpp
│   ├── Socket.cpp
│   ├── TextUtils.cpp
│   └── main.cpp
└── tests/
    ├── integration_test.py
    └── TextUtilsTests.cpp
```

## Requirements

* Linux or WSL
* C++17-compatible compiler
* CMake 3.16 or newer
* POSIX thread support
* Python 3
* Netcat for manual testing

On Ubuntu or WSL, useful packages can be installed with:

```bash
sudo apt update
sudo apt install build-essential cmake python3 netcat-openbsd
```

## Configure

From the project root:

```bash
cmake -S . -B build
```

This tells CMake:

* `-S .` — use the current directory as the source directory
* `-B build` — generate build files inside the `build` directory

## Build

```bash
cmake --build build
```

## Run

Start the server:

```bash
./build/ModernCppChatServer
```

The server listens on port `8080`.

Example output:

```text
[2026-07-13 20:14:32] [INFO] Server listening on port 8080.
[2026-07-13 20:14:32] [INFO] Press Ctrl+C to stop the server.
```

## Connect Manually

Open another terminal and run:

```bash
nc localhost 8080
```

The server prompts:

```text
Enter your username:
```

Enter a unique username and begin sending messages.

Open additional terminals with the same command to simulate multiple clients:

```bash
nc localhost 8080
```

## Example Chat Session

Client 1:

```text
Enter your username: Connor
Welcome, Connor!
Type /help to view available commands.
Hello everyone
Message sent.
```

Client 2:

```text
Enter your username: Alice
Welcome, Alice!
Type /help to view available commands.
Connor: Hello everyone
```

Private-message example:

```text
/msg Alice Hello privately
```

Sender receives:

```text
[Private to Alice] Hello privately
```

Recipient receives:

```text
[Private from Connor] Hello privately
```

## Run Tests

Configure and build the project first:

```bash
cmake -S . -B build
cmake --build build
```

Run all registered tests:

```bash
ctest --test-dir build --output-on-failure
```

Run only the chat-server integration test:

```bash
ctest \
    --test-dir build \
    -R ChatServerIntegration \
    --output-on-failure
```

Run the Python integration test directly:

```bash
python3 \
    tests/integration_test.py \
    ./build/ModernCppChatServer
```

Run only the C++ unit tests:

```bash
ctest \
    --test-dir build \
    -R RemoveLineEndingTest \
    --output-on-failure
```

Run the GoogleTest executable directly

```bash
./build/TextUtilsTests
```

Expected output:

```text
Starting server...
Connecting Connor...
Connecting Alice...
Testing public broadcast...
Testing /users...
Testing private messaging...
Testing /quit...
Testing graceful server shutdown...
Integration test passed.
```

Make sure another server instance is not already running on port `8080` before starting the integration test.

You can check the port with:

```bash
ss -ltnp | grep :8080
```

## Graceful Shutdown

Press:

```text
Ctrl+C
```

in the server terminal.

The server:

1. Receives `SIGINT`.
2. Interrupts the blocking `accept()` call.
3. Stops accepting new clients.
4. Shuts down connected client sockets.
5. Wakes worker threads blocked in `recv()`.
6. Waits for all worker threads using `join()`.
7. Destroys socket resources through RAII.
8. Exits cleanly.

Example shutdown output:

```text
[2026-07-13 20:20:00] [INFO] Shutdown requested. Stopping client connections.
[2026-07-13 20:20:00] [INFO] Connor left the chat.
[2026-07-13 20:20:00] [INFO] Alice left the chat.
[2026-07-13 20:20:00] [INFO] All client threads stopped. Server shutdown complete.
```

## Roadmap

* [x] Project setup
* [x] RAII socket abstraction
* [x] TCP socket creation
* [x] Bind and listen
* [x] Accept client connections
* [x] Receive and send data
* [x] Handle partial sends
* [x] Move-only socket ownership
* [x] Thread-per-client architecture
* [x] Mutex-protected logging
* [x] Encapsulate server lifecycle in a `Server` class
* [x] Track active clients
* [x] Broadcast messages between clients
* [x] Add usernames
* [x] Prevent duplicate usernames
* [x] Add chat commands
* [x] Add private messaging
* [x] Add timestamped structured logging
* [x] Handle `Ctrl+C` gracefully
* [x] Stop and join worker threads during shutdown
* [x] Add integration tests
* [x] Add C++ unit tests
* [ ] Add continuous integration
* [ ] Finalize portfolio documentation
* [ ] Add architecture and data-flow diagrams
