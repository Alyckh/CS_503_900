#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"
#include "rshlib.h"

int exec_local_cmd_loop(int argc, char *argv[]) {
    int server_mode = 0, client_mode = 0;
    char *server_ip = "127.0.0.1";
    int port = 1234;

    // Parse command line arguments to check if server/client mode is specified
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            server_mode = 1;
            printf("Server mode enabled\n");  // Debug output
        } else if (strcmp(argv[i], "-c") == 0) {
            client_mode = 1;
            printf("Client mode enabled\n");  // Debug output
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
            printf("Port set to %d\n", port);  // Debug output
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            server_ip = argv[++i];
            printf("Server IP set to %s\n", server_ip);  // Debug output
        }
    }

    // Handle server and client modes
    if (server_mode) {
        printf("Starting server...\n");  // Debug output
        return run_server(port);
    } else if (client_mode) {
        printf("Starting client...\n");  // Debug output
        return exec_remote_cmd_loop(server_ip, port);
    }

    // Print "localmode" only once at the start of the session
    static int mode_printed = 0;
    if (!mode_printed) {
        printf("localmode\n");
        mode_printed = 1;
    }

    // Now print the prompt twice
    printf("dsh4>dsh4>\n");

    char input[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc;

    // Command execution loop
    while (1) {
        // Always print the prompt before reading the input
        if (isatty(STDIN_FILENO)) {
            printf("dsh4>");
            fflush(stdout);
        }

        // Read user input
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove newline character at the end of input
        input[strcspn(input, "\n")] = '\0';

        // If input is empty, continue to next iteration
        if (strlen(input) == 0) {
            continue;
        }

        // Clear previous command buffer and allocate space for a new command buffer
        clear_cmd_buff(&cmd);
        alloc_cmd_buff(&cmd);

        rc = build_cmd_buff(input, &cmd);
        if (rc == WARN_NO_CMDS) {
            continue;
        }

        // Execute built-in commands (exit, cd, etc.)
        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            continue;
        }

        int pipes[CMD_ARGV_MAX][2];
        pid_t pids[CMD_ARGV_MAX];
        int cmd_count = 0;
        char *commands[CMD_ARGV_MAX];
        char *ptr = strtok(input, "|");

        // Split the input command by pipes
        while (ptr && cmd_count < CMD_ARGV_MAX) {
            commands[cmd_count++] = ptr;
            ptr = strtok(NULL, "|");
        }

        // Execute each command in the pipeline
        for (int i = 0; i < cmd_count; i++) {
            if (i < cmd_count - 1 && pipe(pipes[i]) < 0) {
                perror("pipe failed");
                return ERR_EXEC_CMD;
            }

            if ((pids[i] = fork()) < 0) {
                perror("fork failed");
                return ERR_EXEC_CMD;
            }

            if (pids[i] == 0) {
                // Handle stdin and stdout redirection
                if (i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }
                if (i < cmd_count - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                // Close all pipes in the child process after redirecting
                for (int j = 0; j < i; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Tokenize the command for execution
                char *cmd_argv[CMD_ARGV_MAX];
                int argc = 0;
                ptr = strtok(commands[i], " ");
                while (ptr && argc < CMD_ARGV_MAX - 1) {
                    cmd_argv[argc++] = ptr;
                    ptr = strtok(NULL, " ");
                }
                cmd_argv[argc] = NULL;

                // Execute the command
                execvp(cmd_argv[0], cmd_argv);
                perror("execution failed");
                exit(ERR_EXEC_CMD);
            }

            // Close pipes in the parent process
            if (i > 0) {
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
            }
        }

        // Wait for all child processes to complete
        for (int i = 0; i < cmd_count; i++) {
            waitpid(pids[i], NULL, 0);
        }

        // Always print the prompt before reading the input again
        if (isatty(STDIN_FILENO)) {
            printf("dsh4>");
            fflush(stdout);
        }
    }

    return OK;
}

// In rsh_server.c or dshlib.c if applicable
int run_server(int port) {
    printf("Running server on port %d\n", port);
    return 0;
}


Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    // Handle exit command
    if (strcmp(cmd->argv[0], "exit") == 0) {
        clear_cmd_buff(cmd);
        exit(OK_EXIT);
    }

    // Handle cd command
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1 || chdir(cmd->argv[1]) != 0) {
            fprintf(stderr, "cd: %s: %s\n", cmd->argv[1], strerror(errno));
        }
        return BI_EXECUTED;
    }

    return BI_NOT_BI;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    cmd_buff->argc = 0;
    return OK;
}

bool is_valid_command(const char *cmd) {
    if (!cmd || !*cmd) return false;

    char *path = getenv("PATH");
    if (!path) return false;

    char full_path[EXE_MAX];
    char *dir = strtok(path, ":");

    while (dir) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0) {
            return true;
        }
        dir = strtok(NULL, ":");
    }

    return false;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    cmd_buff->argc = 0;
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    bool in_quotes = false;
    int argc = 0;

    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';

    char *ptr = cmd_buff->_cmd_buffer;

    while (*ptr) {
        while (isspace((unsigned char)*ptr) && !in_quotes) ptr++;

        if (*ptr == '"') {
            in_quotes = !in_quotes;
            ptr++;
            continue;
        }

        if (*ptr == '\0') break;

        cmd_buff->argv[argc++] = ptr;
        while (*ptr && (!isspace((unsigned char)*ptr) || in_quotes)) {
            if (*ptr == '"') {
                memmove(ptr, ptr + 1, strlen(ptr));
                in_quotes = !in_quotes;
                continue;
            }
            ptr++;
        }

        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }

        if (argc >= CMD_ARGV_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;

    return (argc == 0) ? WARN_NO_CMDS : OK;
}
