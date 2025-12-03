#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "rpc_stub.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 4096

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv;
    char filename[256];
    char buffer[BUFFER_SIZE];

    printf("Enter filename: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "rb");
    if (!fp) { printf("File not found.\n"); return 1; }

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);

    connect(sock, (struct sockaddr*)&serv, sizeof(serv));

    // ---- RPC: send filename ----
    rpc_sendFilename(sock, filename);

    // ---- RPC: send file chunks ----
    int n;
    while ((n = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        rpc_sendChunk(sock, buffer, n);
    }

    // ---- RPC: end file ----
    rpc_endFile(sock);

    // ---- Receive ACK ----
    char ack[10] = {0};
    recv(sock, ack, sizeof(ack), 0);
    printf("[CLIENT] Server ACK: %s\n", ack);

    fclose(fp);
    closesocket(sock);
    WSACleanup();
    return 0;
}
