typedef struct {
    int rpc_id;
    int payload_size;
} RpcHeader;

void rpc_sendFilename(SOCKET sock, char *name) {
    RpcHeader h = {1, strlen(name)};
    send(sock, (char*)&h, sizeof(h), 0);
    send(sock, name, strlen(name), 0);
}

void rpc_sendChunk(SOCKET sock, char *buf, int size) {
    RpcHeader h = {2, size};
    send(sock, (char*)&h, sizeof(h), 0);
    send(sock, buf, size, 0);
}

void rpc_endFile(SOCKET sock) {
    RpcHeader h = {3, 0};
    send(sock, (char*)&h, sizeof(h), 0);
}
