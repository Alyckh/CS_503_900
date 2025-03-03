#include "rshlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>

int start_server(char *ifaces, int port, int is_threaded);
int process_cli_requests(int svr_socket);
int exec_client_requests(int cli_socket);
int send_message_string(int cli_socket, char *buff);
void *handle_client(void *arg);

int start_server_main(int argc, char *argv[]) {
    int port = RDSH_DEF_PORT;  // Default port
    int is_threaded = 0;  // Flag for threaded mode (not used in your current code)

    // Parse command-line arguments
    if (argc == 3 && strcmp(argv[1], "-s") == 0) {
        port = atoi(argv[2]);  // Parse port from arguments
        printf("Server mode enabled, running on port %d\n", port);
    } else {
        printf("Usage: %s -s <port>\n", argv[0]);
        return 1;  // Exit with error if the arguments are incorrect
    }

    // Start the server on the specified port
    start_server("127.0.0.1", port, is_threaded);

    return 0;
}

int start_server(char *ifaces, int port, int is_threaded) {
    int server_socket;
    struct sockaddr_in server_addr;
    int opt = 1;

    // Create the server socket (IPv4, TCP)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Server socket creation failed");
        return ERR_RDSH_SERVER;
    }

    // Set socket options to allow reuse of the address
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return ERR_RDSH_SERVER;
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ifaces);  // Use the provided interface

    // Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Server bind failed");
        return ERR_RDSH_SERVER;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) < 0) {
        perror("Server listen failed");
        return ERR_RDSH_SERVER;
    }

    printf("Server started on %s:%d\n", ifaces, port);

    // Accept and handle incoming connections
    while (1) {
        int client_socket;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Accept a client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Server accept failed");
            continue;  // Skip to next iteration if accepting a connection failed
        }

        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        if (is_threaded) {
            pthread_t thread_id;
            // Create a new thread to handle the client
            if (pthread_create(&thread_id, NULL, handle_client, (void*) &client_socket) != 0) {
                perror("Failed to create thread");
                close(client_socket);
            } else {
                // Detach thread so it can clean up resources after it finishes
                pthread_detach(thread_id);
            }
        } else {
            // Handle client requests in the main thread
            process_cli_requests(client_socket);
            close(client_socket);
        }
    }

    close(server_socket);  // Close server socket when done
    return 0;
}

void *handle_client(void *client_socket_ptr) {
    int cli_socket = *((int*)client_socket_ptr);
    exec_client_requests(cli_socket);
    close(cli_socket);
    return NULL;
}

int process_cli_requests(int svr_socket) {
    int cli_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    cli_socket = accept(svr_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (cli_socket < 0) {
        perror("Server: Accept failed");
        return ERR_RDSH_SERVER;
    }

    printf("Client connected.\n");
    exec_client_requests(cli_socket);  // Handle client requests
    close(cli_socket);
    return 0;
}

int exec_client_requests(int cli_socket) {
    char cmd_buff[RDSH_COMM_BUFF_SZ];
    int bytes_received;

    while ((bytes_received = recv(cli_socket, cmd_buff, sizeof(cmd_buff) - 1, 0)) > 0) {
        cmd_buff[bytes_received] = '\0';
        printf("Executing: %s\n", cmd_buff);

        // Check for built-in commands
        if (strcmp(cmd_buff, "exit") == 0) {
            send_message_string(cli_socket, "Goodbye.\n");
            break;
        } else if (strcmp(cmd_buff, "cd") == 0) {
            char *dir = cmd_buff + 3;  // Skip 'cd ' part
            if (chdir(dir) == -1) {
                send_message_string(cli_socket, "cd failed.\n");
            } else {
                send_message_string(cli_socket, "cd success.\n");
            }
            continue;
        }

        // Fork and exec for other commands
        pid_t pid = fork();
        if (pid == 0) {  // Child process
            dup2(cli_socket, STDOUT_FILENO);
            dup2(cli_socket, STDERR_FILENO);
            close(cli_socket);  // Close the socket in child

            execlp(cmd_buff, cmd_buff, (char*)NULL);
            perror("Exec failed");
            exit(1);
        } else if (pid > 0) {  // Parent process
            wait(NULL);  // Wait for the child to finish
            send_message_string(cli_socket, "Command executed.\n");
        } else {
            perror("Fork failed");
        }
    }
    return 0;
}

int send_message_string(int cli_socket, char *buff) {
    send(cli_socket, buff, strlen(buff), 0);
    send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
    return 0;
}
