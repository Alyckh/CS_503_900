#include "rshlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int start_client(char *address, int port) {
    int cli_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((cli_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Client: Socket creation failed");
        return ERR_RDSH_CLIENT;
    }

    // Setup server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        perror("Client: Invalid address");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }

    // Connect to server
    if (connect(cli_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Client: Connection failed");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }

    printf("Connected to server at %s:%d\n", address, port);
    return cli_socket;
}



int send_all(int socket, const char *buffer, size_t length) {
    size_t sent = 0;
    while (sent < length) {
        ssize_t bytes = send(socket, buffer + sent, length - sent, 0);
        if (bytes < 0) {
            perror("Client: Error sending data");
            return ERR_RDSH_CLIENT;
        }
        sent += bytes;
    }
    return 0;
}

int recv_all(int socket, char *buffer, size_t buffer_size) {
    size_t total_received = 0;
    while (total_received < buffer_size - 1) {
        ssize_t bytes = recv(socket, buffer + total_received, 1, 0);
        if (bytes <= 0) {
            return (bytes == 0) ? 0 : ERR_RDSH_CLIENT; // Handle connection closure
        }
        if (buffer[total_received] == RDSH_EOF_CHAR) {
            buffer[total_received] = '\0';
            return total_received;
        }
        total_received++;
    }
    buffer[total_received] = '\0';
    return total_received;
}

int exec_remote_cmd_loop(char *address, int port) {
    int cli_socket = start_client(address, port);
    if (cli_socket < 0) return cli_socket;

    char cmd_buff[RDSH_COMM_BUFF_SZ];
    char rsp_buff[RDSH_COMM_BUFF_SZ];

    while (1) {
        printf("rdsh> ");
        if (fgets(cmd_buff, sizeof(cmd_buff), stdin) == NULL) break;

        // Remove newline character from command
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Handle client-side exit
        if (strcmp(cmd_buff, "exit") == 0) {
            send_all(cli_socket, "exit", 4);
            send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
            printf("Exiting...\n");
            break;
        }

        // Send command to server
        printf("Sending command: %s\n", cmd_buff);
        if (send_all(cli_socket, cmd_buff, strlen(cmd_buff)) < 0) break;
        send(cli_socket, &RDSH_EOF_CHAR, 1, 0);

        // Receive full response from server
        int bytes_received = recv_all(cli_socket, rsp_buff, sizeof(rsp_buff));
        if (bytes_received < 0) {
            fprintf(stderr, "Client: Error receiving response\n");
            break;
        }

        printf("Received response: %s\n", rsp_buff);
    }

    close(cli_socket);
    return 0;
}
