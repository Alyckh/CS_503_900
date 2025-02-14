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
    char input[SH_CMD_MAX];  // Buffer for user input
    cmd_buff_t cmd;          // Command structure
    int rc;                  // Return code

    while (1) {
        printf("%s", SH_PROMPT);  // Display prompt

        if (fgets(input, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';  // Remove newline character

        // Clear and allocate command buffer
        clear_cmd_buff(&cmd);
        alloc_cmd_buff(&cmd);

        // Parse input into cmd_buff_t
        rc = build_cmd_buff(input, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }

        // Handle built-in commands
        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            continue;
        }

        // Handle external commands
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            continue;
        }

        if (pid == 0) {  // Child process
            for (int i = 0; i < cmd.argc; i++) {
                if (strcmp(cmd.argv[i], ">") == 0) {
                    cmd.argv[i] = NULL;  // Terminate command arguments
                    int fd = open(cmd.argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("Redirection failed");
                        exit(ERR_EXEC_CMD);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    break;
                }
            }
            execvp(cmd.argv[0], cmd.argv);
            perror("Execution failed");
            exit(ERR_EXEC_CMD);
        } else {  // Parent process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                return ERR_EXEC_CMD;  // Ensure a non-zero exit code on failure
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
        exit(OK_EXIT);
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return BI_EXECUTED;
        }

        if (chdir(cmd->argv[1]) != 0) {
            perror("cd failed");
            return BI_NOT_BI;  // Return an error indicator
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
