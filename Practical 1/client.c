#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 4096

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char filename[256];
    char buffer[BUFFER_SIZE];

    printf("Enter filename to send: ");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("File not found.\n");
        return 1;
    }

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("socket() failed.\n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("connect() failed.\n");
        return 1;
    }

    printf("[CLIENT] Connected to server.\n");

    send(sock, filename, sizeof(filename), 0);

    int bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sock, buffer, bytes, 0);
    }

    printf("[CLIENT] File sent successfully.\n");

    fclose(fp);
    closesocket(sock);
    WSACleanup();

    return 0;
}
