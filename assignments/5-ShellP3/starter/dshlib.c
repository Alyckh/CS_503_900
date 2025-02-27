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

int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc;

    while (1) {
        printf("%s", SH_PROMPT);
        
        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        // Clear and allocate command buffer
        clear_cmd_buff(&cmd);
        alloc_cmd_buff(&cmd);

        // Parse input
        rc = build_cmd_buff(input, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }

        // Handle built-in commands
        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            continue;
        }

        // Check for pipe operator '|'
        char *cmd1_argv[CMD_ARGV_MAX], *cmd2_argv[CMD_ARGV_MAX];
        int has_pipe = 0;
        for (int i = 0; i < cmd.argc; i++) {
            if (strcmp(cmd.argv[i], "|") == 0) {
                cmd.argv[i] = NULL;  // Terminate first command
                has_pipe = 1;

                // Split into two command arrays
                memcpy(cmd1_argv, cmd.argv, (i + 1) * sizeof(char *));
                memcpy(cmd2_argv, cmd.argv + i + 1, (cmd.argc - i) * sizeof(char *));
                cmd2_argv[cmd.argc - i - 1] = NULL;
                break;
            }
        }

        if (has_pipe) {
            // Create a pipe
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe failed");
                continue;
            }

            // Fork first child process for left-hand command
            pid_t pid1 = fork();
            if (pid1 < 0) {
                perror("fork failed");
                continue;
            }

            if (pid1 == 0) { // Child 1
                close(pipefd[0]);   // Close unused read end
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);

                execvp(cmd1_argv[0], cmd1_argv);
                perror("execution failed");
                exit(ERR_EXEC_CMD);
            }

            // Fork second child process for right-hand command
            pid_t pid2 = fork();
            if (pid2 < 0) {
                perror("fork failed");
                continue;
            }

            if (pid2 == 0) { // Child 2
                close(pipefd[1]);   // Close unused write end
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);

                execvp(cmd2_argv[0], cmd2_argv);
                perror("execution failed");
                exit(ERR_EXEC_CMD);
            }

            // Parent process
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        } else {
            // Execute single command
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                continue;
            }

            if (pid == 0) {  // Child process
                execvp(cmd.argv[0], cmd.argv);
                perror("Execution failed");
                exit(ERR_EXEC_CMD);
            } else {  // Parent process
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }

    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        clear_cmd_buff(cmd);
        exit(OK_EXIT);
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return BI_EXECUTED;
        }

        if (chdir(cmd->argv[1]) != 0) {
            perror("cd failed");
        }
        return BI_EXECUTED;
    }

    return BI_NOT_BI;
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

    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;

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
