#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 4096

typedef struct {
    int rpc_id;
    int payload_size;
} RpcHeader;

FILE *fp = NULL;

void handle_rpc(SOCKET client, RpcHeader h) {
    char buffer[BUFFER_SIZE];

    if (h.rpc_id == 1) {              // ---- RPC: beginFile ----
        recv(client, buffer, h.payload_size, 0);
        buffer[h.payload_size] = '\0';

        printf("[SERVER] File name: %s\n", buffer);

        fp = fopen(buffer, "wb");
        if (!fp) {
            printf("Cannot create file.\n");
            return;
        }
    }

    else if (h.rpc_id == 2) {         // ---- RPC: sendChunk ----
        recv(client, buffer, h.payload_size, 0);
        fwrite(buffer, 1, h.payload_size, fp);
    }

    else if (h.rpc_id == 3) {         // ---- RPC: endFile ----
        fclose(fp);
        printf("[SERVER] File saved.\n");

        send(client, "OK", 2, 0);
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    WSAStartup(MAKEWORD(2,2), &wsa);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    printf("[SERVER] Waiting for connection...\n");
    client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    printf("[SERVER] Client connected.\n");

    RpcHeader header;

    while (1) {
        int n = recv(client_fd, (char*)&header, sizeof(header), 0);
        if (n <= 0) break;
        handle_rpc(client_fd, header);
    }

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
