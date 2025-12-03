#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 5000
#define BUFFER_SIZE 4096

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char filename[256];
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("socket() failed.\n");
        return 1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("bind() failed.\n");
        return 1;
    }

    if (listen(server_fd, 1) == SOCKET_ERROR) {
        printf("listen() failed.\n");
        return 1;
    }

    printf("[SERVER] Listening on port %d...\n", PORT);

    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket == INVALID_SOCKET) {
        printf("accept() failed.\n");
        return 1;
    }

    printf("[SERVER] Client connected.\n");

    recv(new_socket, filename, sizeof(filename), 0);
    printf("[SERVER] Receiving file: %s\n", filename);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Could not create file.\n");
        return 1;
    }

    int bytes;
    while ((bytes = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes, fp);
    }

    printf("[SERVER] File received successfully.\n");

    fclose(fp);
    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
