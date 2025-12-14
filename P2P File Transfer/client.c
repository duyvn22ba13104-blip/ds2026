#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 9000
#define NODE_PORT   10000
#define BUF_SIZE    1024
#define MAX_NAME    256

/* Message types (matches server.c) */
#define MSG_REGISTER 1
#define MSG_LOOKUP   2
#define MSG_REPLY    3

typedef struct {
    char filename[MAX_NAME];
    char node_ip[INET_ADDRSTRLEN];
    int  node_port;
} FileEntry;

typedef struct {
    int type;
    FileEntry entry;
} Message;

/* Serve file to another node */
void serve_file(const char *filename) {
    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NODE_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 5);

    printf("Sharing file: %s\n", filename);

    while (1) {
        int client = accept(server, NULL, NULL);

        FILE *fp = fopen(filename, "rb");
        char buffer[BUF_SIZE];
        int n;

        while ((n = fread(buffer, 1, BUF_SIZE, fp)) > 0)
            send(client, buffer, n, 0);

        fclose(fp);
        close(client);
    }
}

/* Register file with discovery server */
void register_file(const char *filename) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_aton("127.0.0.1", &server.sin_addr);

    Message msg;
    msg.type = MSG_REGISTER;
    strcpy(msg.entry.filename, filename);
    strcpy(msg.entry.node_ip, "127.0.0.1");
    msg.entry.node_port = NODE_PORT;

    sendto(sock, &msg, sizeof(msg), 0,
           (struct sockaddr*)&server, sizeof(server));

    close(sock);
}

/* Lookup and download file */
void download_file(const char *filename) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_aton("127.0.0.1", &server.sin_addr);

    Message msg;
    msg.type = MSG_LOOKUP;
    strcpy(msg.entry.filename, filename);

    sendto(sock, &msg, sizeof(msg), 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(sock, &msg, sizeof(msg), 0, NULL, NULL);
    close(sock);

    if (msg.entry.node_port == 0) {
        printf("File not found\n");
        return;
    }

    int peer = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in peer_addr;

    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(msg.entry.node_port);
    inet_aton(msg.entry.node_ip, &peer_addr.sin_addr);

    connect(peer, (struct sockaddr*)&peer_addr, sizeof(peer_addr));

    FILE *fp = fopen(filename, "wb");
    char buffer[BUF_SIZE];
    int n;

    while ((n = recv(peer, buffer, BUF_SIZE, 0)) > 0)
        fwrite(buffer, 1, n, fp);

    fclose(fp);
    close(peer);

    printf("Download complete: %s\n", filename);
}

int main() {
    int choice;
    char filename[MAX_NAME];

    printf("1. Share file\n2. Download file\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("File name: ");
        scanf("%s", filename);
        register_file(filename);
        serve_file(filename);
    } else {
        printf("File name: ");
        scanf("%s", filename);
        download_file(filename);
    }

    return 0;
}
