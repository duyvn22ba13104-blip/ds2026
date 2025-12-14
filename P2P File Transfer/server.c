#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 9000
#define MAX_FILES   100
#define MAX_NAME    256

/* Message types */
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

FileEntry database[MAX_FILES];
int file_count = 0;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Server running on port %d...\n", SERVER_PORT);

    Message msg;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        recvfrom(sock, &msg, sizeof(msg), 0,
                 (struct sockaddr*)&client_addr, &addr_len);

        if (msg.type == MSG_REGISTER) {
            database[file_count++] = msg.entry;

            printf("Registered: %s from %s:%d\n",
                   msg.entry.filename,
                   msg.entry.node_ip,
                   msg.entry.node_port);
        }

        else if (msg.type == MSG_LOOKUP) {
            Message reply;
            reply.type = MSG_REPLY;
            memset(&reply.entry, 0, sizeof(FileEntry));

            for (int i = 0; i < file_count; i++) {
                if (strcmp(database[i].filename,
                           msg.entry.filename) == 0) {
                    reply.entry = database[i];
                    break;
                }
            }

            sendto(sock, &reply, sizeof(reply), 0,
                   (struct sockaddr*)&client_addr, addr_len);
        }
    }

    close(sock);
    return 0;
}
