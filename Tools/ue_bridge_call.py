#!/usr/bin/env python3
"""Minimal WebSocket JSON-RPC client for UE-MCP bridge (ws://localhost:9877)."""
import json
import socket
from pathlib import Path
import struct
import sys
import base64
import secrets
import hashlib

HOST = "127.0.0.1"
PORT = 9877


def ws_handshake(sock: socket.socket) -> None:
    key = base64.b64encode(secrets.token_bytes(16)).decode()
    req = (
        f"GET / HTTP/1.1\r\n"
        f"Host: {HOST}:{PORT}\r\n"
        f"Upgrade: websocket\r\n"
        f"Connection: Upgrade\r\n"
        f"Sec-WebSocket-Key: {key}\r\n"
        f"Sec-WebSocket-Version: 13\r\n\r\n"
    )
    sock.sendall(req.encode())
    resp = b""
    while b"\r\n\r\n" not in resp:
        resp += sock.recv(4096)
    if b"101" not in resp.split(b"\r\n", 1)[0]:
        raise RuntimeError(f"WebSocket handshake failed:\n{resp[:500]!r}")


def ws_send(sock: socket.socket, text: str) -> None:
    data = text.encode("utf-8")
    mask = secrets.token_bytes(4)
    frame = bytearray([0x81])
    n = len(data)
    if n < 126:
        frame.append(0x80 | n)
    elif n < 65536:
        frame.append(0x80 | 126)
        frame.extend(struct.pack(">H", n))
    else:
        frame.append(0x80 | 127)
        frame.extend(struct.pack(">Q", n))
    frame.extend(mask)
    frame.extend(bytes(b ^ mask[i % 4] for i, b in enumerate(data)))
    sock.sendall(frame)


def ws_recv(sock: socket.socket) -> str:
    def read_exact(n: int) -> bytes:
        buf = b""
        while len(buf) < n:
            chunk = sock.recv(n - len(buf))
            if not chunk:
                raise RuntimeError("Connection closed")
            buf += chunk
        return buf

    b1, b2 = read_exact(2)
    opcode = b1 & 0x0F
    masked = bool(b2 & 0x80)
    length = b2 & 0x7F
    if length == 126:
        length = struct.unpack(">H", read_exact(2))[0]
    elif length == 127:
        length = struct.unpack(">Q", read_exact(8))[0]
    if masked:
        mask = read_exact(4)
    payload = read_exact(length)
    if masked:
        payload = bytes(b ^ mask[i % 4] for i, b in enumerate(payload))
    if opcode == 0x8:
        return ""
    if opcode == 0x9:
        sock.sendall(bytes([0x8A, 0x80]) + secrets.token_bytes(4) + payload)
        return ws_recv(sock)
    return payload.decode("utf-8", errors="replace")


def call(method: str, params: dict | None = None, req_id: int = 1) -> dict:
    sock = socket.create_connection((HOST, PORT), timeout=120)
    try:
        ws_handshake(sock)
        msg = {"jsonrpc": "2.0", "id": req_id, "method": method, "params": params or {}}
        ws_send(sock, json.dumps(msg))
        raw = ws_recv(sock)
        return json.loads(raw)
    finally:
        try:
            sock.close()
        except OSError:
            pass


def main() -> None:
    if len(sys.argv) < 2:
        print("Usage: ue_bridge_call.py <method> [json_params_or_file]")
        sys.exit(1)
    method = sys.argv[1]
    params: dict = {}
    if len(sys.argv) > 2:
        arg = sys.argv[2]
        if arg.endswith(".json") and Path(arg).is_file():
            params = json.loads(Path(arg).read_text(encoding="utf-8"))
        else:
            params = json.loads(arg)
    result = call(method, params)
    print(json.dumps(result, indent=2, ensure_ascii=False))


if __name__ == "__main__":
    main()
