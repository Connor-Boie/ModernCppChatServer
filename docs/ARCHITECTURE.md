# Architecture

This document describes the architecture and runtime behavior of the Modern C++ TCP Chat Server.

## System Overview

The project is a multithreaded TCP chat server written in C++17 using Linux/POSIX socket APIs.

The server:

- Listens for TCP connections on port `8080`
- Accepts multiple connected clients
- Creates one worker thread per client
- Registers a unique username for each client
- Broadcasts public messages
- Routes private messages by username
- Processes chat commands
- Coordinates graceful shutdown after receiving `SIGINT`

## High-Level Architecture

```mermaid
flowchart LR
    ClientA[Client A]
    ClientB[Client B]
    ClientC[Client C]

    Listener[Listening Socket]
    Server[Server]
    WorkerA[Worker Thread A]
    WorkerB[Worker Thread B]
    WorkerC[Worker Thread C]

    ClientStore[Active Client Collection]
    UsernameStore[Username Registry]
    UsernameMap[Username-to-Socket Map]

    ClientA <-->|TCP| Listener
    ClientB <-->|TCP| Listener
    ClientC <-->|TCP| Listener

    Listener --> Server

    Server --> WorkerA
    Server --> WorkerB
    Server --> WorkerC

    WorkerA --> ClientStore
    WorkerB --> ClientStore
    WorkerC --> ClientStore

    WorkerA --> UsernameStore
    WorkerB --> UsernameStore
    WorkerC --> UsernameStore

    WorkerA --> UsernameMap
    WorkerB --> UsernameMap
    WorkerC --> UsernameMap
```

The listening socket accepts incoming connections. Each accepted client socket is placed in shared client storage and handled by a dedicated worker thread.

## Main Components

### `main.cpp`

`main.cpp` is the program entry point.

Its responsibilities are intentionally small:

1. Construct a `Server` object.
2. Start the server with `Server::run()`.
3. Catch exceptions that escape the server.
4. Return an appropriate process exit code.

```mermaid
flowchart TD
    Start[Program starts]
    Construct[Construct Server]
    Run[Call Server::run]
    Error{Exception thrown?}
    Report[Print server error]
    Success[Return 0]
    Failure[Return 1]

    Start --> Construct
    Construct --> Run
    Run --> Error
    Error -- No --> Success
    Error -- Yes --> Report
    Report --> Failure
```

### `Socket`

The `Socket` class owns one operating-system socket file descriptor.

Its responsibilities include:

- Creating a socket
- Binding to a port
- Listening for connections
- Accepting client sockets
- Sending data
- Receiving data
- Shutting down socket operations
- Closing the file descriptor during destruction

The class follows RAII. When a `Socket` object is destroyed, it closes its owned file descriptor automatically.

The class is move-only:

- Copy construction is deleted.
- Copy assignment is deleted.
- Move construction transfers ownership.
- Move assignment transfers ownership.

This prevents two `Socket` objects from accidentally believing that they exclusively own the same file descriptor.

```mermaid
flowchart LR
    FD[Operating-System File Descriptor]
    SocketObject[Socket Object]
    Destructor[Socket Destructor]
    Close[close system call]

    SocketObject -->|owns| FD
    SocketObject --> Destructor
    Destructor --> Close
    Close --> FD
```

### `Server`

The `Server` class manages the complete chat-server lifecycle.

Its responsibilities include:

- Owning the listening socket
- Accepting new client connections
- Creating client worker threads
- Tracking connected clients
- Registering and removing usernames
- Broadcasting public messages
- Routing private messages
- Processing commands
- Logging server activity
- Coordinating shutdown
- Joining worker threads before destruction

### `TextUtils`

`TextUtils` contains text-processing logic that does not depend on the server or networking system.

Currently it provides:

```cpp
text_utils::removeLineEnding(...)
```

The function removes trailing carriage-return and newline characters from received text.

Keeping this logic separate allows it to be tested independently with GoogleTest.

## Source-Code Dependency Diagram

```mermaid
flowchart TD
    Main[src/main.cpp]
    ServerSource[src/Server.cpp]
    SocketSource[src/Socket.cpp]
    TextSource[src/TextUtils.cpp]

    ServerHeader[include/Server.h]
    SocketHeader[include/Socket.h]
    TextHeader[include/TextUtils.h]

    Main --> ServerHeader

    ServerSource --> ServerHeader
    ServerSource --> SocketHeader
    ServerSource --> TextHeader

    SocketSource --> SocketHeader
    TextSource --> TextHeader
```

## Connection Lifecycle

When a new client connects:

1. The listening socket accepts the connection.
2. A new `Socket` object owns the accepted file descriptor.
3. The socket is stored in a `std::shared_ptr`.
4. The shared pointer is added to the active-client collection.
5. A worker thread is created for the client.
6. The worker asks the client for a username.
7. The username is registered if it is unique.
8. The client enters the message-processing loop.

```mermaid
sequenceDiagram
    participant Client
    participant Listener as Listening Socket
    participant Server
    participant Worker as Client Worker Thread
    participant Registry as Username Registry

    Client->>Listener: TCP connection request
    Listener->>Server: accept()
    Server->>Server: Create shared Socket
    Server->>Server: Add active client
    Server->>Worker: Start worker thread
    Worker->>Client: Enter your username
    Client->>Worker: Username
    Worker->>Registry: Attempt registration

    alt Username is available
        Registry-->>Worker: Registration successful
        Worker->>Client: Welcome message
    else Username is already active
        Registry-->>Worker: Registration failed
        Worker->>Client: Request another username
    end
```

## Worker-Thread Model

The server uses a thread-per-client architecture.

Each connected client has a worker thread that:

- Waits for incoming messages
- Cleans received line endings
- Detects commands
- Broadcasts public messages
- Routes private messages
- Removes the client during disconnection

```mermaid
flowchart TD
    Start[Worker starts]
    Register[Register username]
    Receive[Wait in receive]
    Empty{Connection closed?}
    Command{Starts with slash?}
    ProcessCommand[Process command]
    Broadcast[Broadcast public message]
    Quit{Should disconnect?}
    Cleanup[Remove username and client]
    Finish[Worker exits]

    Start --> Register
    Register --> Receive
    Receive --> Empty

    Empty -- Yes --> Cleanup
    Empty -- No --> Command

    Command -- Yes --> ProcessCommand
    Command -- No --> Broadcast

    ProcessCommand --> Quit
    Broadcast --> Receive

    Quit -- Yes --> Cleanup
    Quit -- No --> Receive

    Cleanup --> Finish
```

## Public-Message Flow

When Connor sends a public message:

```text
Hello everyone
```

the worker thread formats it as:

```text
Connor: Hello everyone
```

The server sends the formatted message to every active client except Connor.

```mermaid
sequenceDiagram
    participant Connor
    participant ConnorWorker as Connor Worker
    participant Server
    participant Alice
    participant Bob

    Connor->>ConnorWorker: Hello everyone
    ConnorWorker->>Server: broadcast("Connor: Hello everyone")
    Server->>Alice: Connor: Hello everyone
    Server->>Bob: Connor: Hello everyone
    ConnorWorker->>Connor: Message sent
```

The sender is excluded by comparing socket file descriptors.

## Private-Message Flow

A private message uses the command:

```text
/msg Alice Secret message
```

The worker thread:

1. Parses the recipient username.
2. Parses the message body.
3. Looks up the recipient in the username-to-socket map.
4. Copies the recipient’s shared pointer.
5. Releases the username mutex.
6. Sends the private message.
7. Sends confirmation to the sender.

```mermaid
sequenceDiagram
    participant Connor
    participant ConnorWorker as Connor Worker
    participant Map as Username-to-Socket Map
    participant Alice

    Connor->>ConnorWorker: /msg Alice Secret message
    ConnorWorker->>Map: Find "Alice"

    alt Alice is connected
        Map-->>ConnorWorker: Alice socket
        ConnorWorker->>Alice: [Private from Connor] Secret message
        ConnorWorker->>Connor: [Private to Alice] Secret message
    else Alice is not connected
        Map-->>ConnorWorker: Not found
        ConnorWorker->>Connor: User is unavailable
    end
```

## Command Processing

Supported commands are:

```text
/help
/users
/msg <username> <message>
/quit
```

Command routing follows this general flow:

```mermaid
flowchart TD
    Input[Received client message]
    Help{Is /help?}
    Users{Is /users?}
    Message{Starts with /msg?}
    Quit{Is /quit?}

    HelpResponse[Send command list]
    UsersResponse[Send connected users]
    PrivateResponse[Parse and route private message]
    QuitResponse[Send goodbye and disconnect]
    Unknown[Send unknown-command response]

    Input --> Help
    Help -- Yes --> HelpResponse
    Help -- No --> Users

    Users -- Yes --> UsersResponse
    Users -- No --> Message

    Message -- Yes --> PrivateResponse
    Message -- No --> Quit

    Quit -- Yes --> QuitResponse
    Quit -- No --> Unknown
```

## Shared State and Synchronization

Multiple worker threads access shared server data. Mutexes protect this data from concurrent modification.

### Client mutex

The client mutex protects:

```cpp
std::vector<std::shared_ptr<Socket>> m_clients;
```

This collection contains active client sockets.

Operations protected by the client mutex include:

- Adding a connected client
- Removing a disconnected client
- Iterating over active clients
- Copying active clients during shutdown

### Username mutex

The username mutex protects the relationship between:

```cpp
std::unordered_set<std::string> m_usernames;
```

and:

```cpp
std::unordered_map<
    std::string,
    std::shared_ptr<Socket>
> m_userSockets;
```

These containers share one logical invariant:

> A registered username should have a corresponding socket mapping.

Using one mutex allows registration and removal to update both containers as one protected operation.

### Output mutex

The output mutex protects console logging.

Without it, messages from several worker threads could become interleaved:

```text
[INFO] Conn[WARNING] Alice disor joined...
```

With the mutex, one complete log entry is written at a time.

## Mutex Ownership Diagram

```mermaid
flowchart LR
    ClientsMutex[m_clientsMutex]
    UsernameMutex[m_usernamesMutex]
    OutputMutex[m_outputMutex]

    Clients[m_clients]
    Usernames[m_usernames]
    UserSockets[m_userSockets]
    Console[Console Output]

    ClientsMutex -->|protects| Clients

    UsernameMutex -->|protects| Usernames
    UsernameMutex -->|protects| UserSockets

    OutputMutex -->|protects| Console
```

The design uses mutexes by shared responsibility rather than assigning one mutex to every individual variable.

## Graceful-Shutdown Flow

The server handles `Ctrl+C` through `SIGINT`.

The signal handler performs only a minimal operation:

```text
Set the shutdown-requested flag
```

The normal server control flow performs the actual cleanup.

Shutdown proceeds as follows:

1. The process receives `SIGINT`.
2. The signal handler sets the shutdown flag.
3. The blocking `accept()` operation is interrupted.
4. The server stops accepting new clients.
5. The server calls `shutdown()` on active client sockets.
6. Worker threads blocked in `recv()` wake up.
7. Each worker exits its message loop.
8. Workers remove their usernames and client entries.
9. The server joins every worker thread.
10. RAII closes remaining socket file descriptors.
11. The process exits.

```mermaid
sequenceDiagram
    participant User
    participant SignalHandler as SIGINT Handler
    participant Server
    participant ClientSockets as Active Client Sockets
    participant Workers as Worker Threads

    User->>SignalHandler: Ctrl+C
    SignalHandler->>SignalHandler: Set shutdown flag
    SignalHandler-->>Server: accept() is interrupted
    Server->>Server: Stop accept loop
    Server->>ClientSockets: shutdown()
    ClientSockets-->>Workers: recv() wakes
    Workers->>Workers: Exit processing loops
    Workers->>Server: Remove client state
    Server->>Workers: join()
    Workers-->>Server: All workers finished
    Server->>Server: Complete RAII cleanup
```

## Why Client Sockets Use `std::shared_ptr`

A connected client socket may be referenced simultaneously by:

- The active-client collection
- Its worker thread
- The username-to-socket map
- A broadcast operation
- A private-message operation
- The shutdown process

`std::shared_ptr<Socket>` allows these operations to temporarily share ownership safely.

The socket is destroyed only after the final shared pointer is released.

```mermaid
flowchart TD
    Socket[Client Socket Object]

    ClientList[m_clients]
    Worker[Worker Thread]
    UsernameMap[m_userSockets]
    Temporary[Temporary Operation Copy]

    ClientList -->|shared ownership| Socket
    Worker -->|shared ownership| Socket
    UsernameMap -->|shared ownership| Socket
    Temporary -->|shared ownership| Socket
```

This shared ownership is different from the ownership of the underlying file descriptor.

The `Socket` object has shared ownership, but the file descriptor itself is still exclusively owned by that one `Socket` object.

## Testing Architecture

The project uses different tools for different testing levels.

### C++ unit tests

GoogleTest verifies isolated C++ logic such as line-ending cleanup.

```text
TextUtilsTests.cpp
        |
        v
TextUtils.cpp
```

### Python integration test

The Python test treats the server as an external program.

It:

- Starts the server process
- Connects real TCP clients
- Registers usernames
- Sends public messages
- Sends private messages
- Executes commands
- Sends `SIGINT`
- Verifies clean process termination

```mermaid
flowchart LR
    CTest[CTest]
    UnitTests[GoogleTest Unit Tests]
    Integration[Python Integration Test]
    Server[Chat Server Process]
    Clients[Python TCP Clients]

    CTest --> UnitTests
    CTest --> Integration
    Integration --> Server
    Integration --> Clients
    Clients <-->|TCP| Server
```

### Continuous integration

GitHub Actions runs the complete test suite in a clean Ubuntu environment.

```mermaid
flowchart LR
    Commit[Push or Pull Request]
    Actions[GitHub Actions]
    Configure[CMake Configure]
    Build[C++ Build]
    Test[CTest]
    Result[Pass or Fail]

    Commit --> Actions
    Actions --> Configure
    Configure --> Build
    Build --> Test
    Test --> Result
```

## Design Tradeoffs

### Thread per client

Advantages:

- Straightforward control flow
- Easy to understand
- Each client can block independently in `recv()`
- Appropriate for a learning and portfolio project

Tradeoffs:

- One operating-system thread is created per connected client
- Thread count does not scale as efficiently as an event-driven server
- More advanced production servers may use nonblocking I/O or thread pools

### Shared pointers for clients

Advantages:

- Simplifies lifetime management across threads and containers
- Prevents sockets from being destroyed while another operation uses them

Tradeoffs:

- Ownership is less obvious than with unique ownership
- Reference counting introduces some overhead
- Care is required to avoid ownership cycles

This project does not create a shared-pointer cycle because the `Socket` objects do not own the `Server` or the containers that reference them.

### POSIX APIs

Advantages:

- Provides direct experience with Linux networking
- Exposes file descriptors and system-call behavior
- Relevant to systems and embedded-adjacent C++ development

Tradeoffs:

- The current networking layer is Linux/POSIX-specific
- Portability to Windows would require a platform abstraction or Winsock implementation

## Potential Future Improvements

Possible future extensions include:

- Message framing instead of relying on newline-delimited input
- Per-client outgoing-message queues
- A fixed-size worker pool
- Nonblocking I/O using `poll`, `select`, or `epoll`
- Configurable host and port settings
- Authentication
- Encrypted communication
- Additional unit tests for command parsing
- Stress and concurrency tests
- Sanitizer builds
- Multiple compiler configurations in continuous integration