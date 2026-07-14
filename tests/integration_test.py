#!/usr/bin/env python3

import signal
import socket
import subprocess
import sys
import time
from pathlib import Path


HOST = "127.0.0.1"
PORT = 8080
SOCKET_TIMEOUT_SECONDS = 3.0
SERVER_START_TIMEOUT_SECONDS = 5.0
SERVER_STOP_TIMEOUT_SECONDS = 5.0


class TestFailure(Exception):
    """Raised when an integration-test expectation is not met."""


def receive_until(
    client: socket.socket,
    expected_text: str,
    timeout_seconds: float = SOCKET_TIMEOUT_SECONDS,
) -> str:
    """Receive data until expected_text appears or the timeout expires."""

    deadline = time.monotonic() + timeout_seconds
    received = ""

    while expected_text not in received:
        remaining = deadline - time.monotonic()

        if remaining <= 0:
            raise TestFailure(
                f"Timed out waiting for {expected_text!r}.\n"
                f"Received so far:\n{received}"
            )

        client.settimeout(remaining)

        try:
            data = client.recv(4096)
        except socket.timeout as error:
            raise TestFailure(
                f"Timed out waiting for {expected_text!r}.\n"
                f"Received so far:\n{received}"
            ) from error

        if not data:
            raise TestFailure(
                f"Connection closed while waiting for "
                f"{expected_text!r}.\n"
                f"Received so far:\n{received}"
            )

        received += data.decode("utf-8", errors="replace")

    return received


def receive_until_all(
    client: socket.socket,
    expected_texts: list[str],
    timeout_seconds: float = SOCKET_TIMEOUT_SECONDS,
) -> str:
    """Receive data until every expected string appears."""

    deadline = time.monotonic() + timeout_seconds
    received = ""

    while not all(text in received for text in expected_texts):
        remaining = deadline - time.monotonic()

        if remaining <= 0:
            raise TestFailure(
                "Timed out waiting for all expected text.\n"
                f"Expected: {expected_texts}\n"
                f"Received so far:\n{received}"
            )

        client.settimeout(remaining)

        try:
            data = client.recv(4096)
        except socket.timeout as error:
            raise TestFailure(
                "Timed out waiting for all expected text.\n"
                f"Expected: {expected_texts}\n"
                f"Received so far:\n{received}"
            ) from error

        if not data:
            raise TestFailure(
                "Connection closed before all expected text arrived.\n"
                f"Expected: {expected_texts}\n"
                f"Received so far:\n{received}"
            )

        received += data.decode("utf-8", errors="replace")

    return received


def send_line(client: socket.socket, text: str) -> None:
    """Send one newline-terminated message."""

    client.sendall((text + "\n").encode("utf-8"))


def connect_client(username: str) -> socket.socket:
    """Connect one client and complete username registration."""

    client = socket.create_connection(
        (HOST, PORT),
        timeout=SOCKET_TIMEOUT_SECONDS,
    )

    receive_until(
        client,
        "Enter your username: ",
    )

    send_line(client, username)

    receive_until_all(
        client,
        [
            f"Welcome, {username}!",
            "Type /help to view available commands.",
        ],
    )

    return client


def wait_for_server_start(
    server_process: subprocess.Popen[str],
) -> None:
    """Wait until the server reports that it is listening."""

    if server_process.stdout is None:
        raise TestFailure(
            "Server standard output was not captured."
        )

    deadline = (
        time.monotonic()
        + SERVER_START_TIMEOUT_SECONDS
    )

    output = ""

    while time.monotonic() < deadline:
        if server_process.poll() is not None:
            remaining_output = server_process.stdout.read()

            raise TestFailure(
                "Server exited before it started listening.\n"
                f"Server output:\n"
                f"{output}{remaining_output}"
            )

        line = server_process.stdout.readline()

        if line:
            output += line

            if "Server listening on port" in line:
                return
        else:
            time.sleep(0.05)

    raise TestFailure(
        "Timed out waiting for the server to start.\n"
        f"Server output:\n{output}"
    )


def close_client(client: socket.socket | None) -> None:
    """Close a test client without raising cleanup errors."""

    if client is None:
        return

    try:
        client.shutdown(socket.SHUT_RDWR)
    except OSError:
        pass

    try:
        client.close()
    except OSError:
        pass


def stop_server(
    server_process: subprocess.Popen[str] | None,
) -> None:
    """Stop the server during cleanup if it is still running."""

    if server_process is None:
        return

    if server_process.poll() is not None:
        return

    server_process.send_signal(signal.SIGINT)

    try:
        server_process.wait(
            timeout=SERVER_STOP_TIMEOUT_SECONDS
        )
    except subprocess.TimeoutExpired:
        server_process.kill()
        server_process.wait()


def run_test(server_executable: Path) -> None:
    """Run the complete chat-server integration test."""

    server_process: subprocess.Popen[str] | None = None
    connor: socket.socket | None = None
    alice: socket.socket | None = None

    try:
        print("Starting server...")

        server_process = subprocess.Popen(
            [str(server_executable)],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,
        )

        wait_for_server_start(server_process)

        print("Connecting Connor...")
        connor = connect_client("Connor")

        print("Connecting Alice...")
        alice = connect_client("Alice")

        receive_until(
            connor,
            "*** Alice joined the chat. ***",
        )

        print("Testing public broadcast...")

        send_line(
            connor,
            "Hello from Connor",
        )

        receive_until(
            connor,
            "Message sent.",
        )

        receive_until(
            alice,
            "Connor: Hello from Connor",
        )

        print("Testing /users...")

        send_line(
            connor,
            "/users",
        )

        receive_until_all(
            connor,
            [
                "Connected users (2):",
                "Connor",
                "Alice",
            ],
        )

        print("Testing private messaging...")

        send_line(
            connor,
            "/msg Alice Secret message",
        )

        receive_until(
            connor,
            "[Private to Alice] Secret message",
        )

        receive_until(
            alice,
            "[Private from Connor] Secret message",
        )

        print("Testing /quit...")

        send_line(
            alice,
            "/quit",
        )

        receive_until(
            alice,
            "Goodbye!",
        )

        receive_until(
            connor,
            "*** Alice left the chat. ***",
        )

        print("Testing graceful server shutdown...")

        server_process.send_signal(signal.SIGINT)

        try:
            exit_code = server_process.wait(
                timeout=SERVER_STOP_TIMEOUT_SECONDS
            )
        except subprocess.TimeoutExpired as error:
            raise TestFailure(
                "Server did not stop after receiving SIGINT."
            ) from error

        if exit_code != 0:
            server_output = ""

            if server_process.stdout is not None:
                server_output = server_process.stdout.read()

            raise TestFailure(
                f"Server exited with code {exit_code}.\n"
                f"Server output:\n{server_output}"
            )

        print("Integration test passed.")

    finally:
        close_client(alice)
        close_client(connor)
        stop_server(server_process)


def main() -> int:
    if len(sys.argv) != 2:
        print(
            "Usage: integration_test.py "
            "<server-executable>",
            file=sys.stderr,
        )

        return 2

    server_executable = Path(sys.argv[1]).resolve()

    if not server_executable.is_file():
        print(
            "Server executable does not exist: "
            f"{server_executable}",
            file=sys.stderr,
        )

        return 2

    try:
        run_test(server_executable)
    except TestFailure as error:
        print(
            f"Integration test failed:\n{error}",
            file=sys.stderr,
        )

        return 1
    except Exception as error:
        print(
            "Unexpected integration-test error: "
            f"{error}",
            file=sys.stderr,
        )

        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())