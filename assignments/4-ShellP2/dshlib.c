#include "dshlib.h"
#include <ctype.h>

void init_cmd_list(cmd_list_t *cmd_list) {
    cmd_list->num_commands = 0;
    for (int i = 0; i < MAX_COMMANDS; i++) {
        cmd_list->commands[i].num_args = 0;
        for (int j = 0; j < MAX_TOKENS; j++) {
            cmd_list->commands[i].args[j] = NULL;
        }
    }
}

int is_whitespace_only(const char *str) {
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int build_cmd_list(cmd_list_t *cmd_list, const char *input) {
    if (!input || strlen(input) == 0 || is_whitespace_only(input)) return -1;

    char *input_copy = strdup(input);
    if (!input_copy) return -1;

    int cmd_idx = 0;
    char *saveptr;
    char *cmd_token = strtok_r(input_copy, "|", &saveptr);

    while (cmd_token) {
        if (cmd_idx >= MAX_COMMANDS) {
            printf("error: piping limited to %d commands\n", MAX_COMMANDS);
            free(input_copy);
            cmd_list->num_commands = 0;  // Ensure the command list is empty
            return -1; // Too many commands
        }

        char *arg_saveptr;
        char *arg_token = strtok_r(cmd_token, " \t\n", &arg_saveptr);
        int arg_idx = 0;

        while (arg_token) {
            if (arg_idx >= MAX_TOKENS - 1) break; // Prevent overflow

            cmd_list->commands[cmd_idx].args[arg_idx++] = strdup(arg_token);
            arg_token = strtok_r(NULL, " \t\n", &arg_saveptr);
        }

        cmd_list->commands[cmd_idx].args[arg_idx] = NULL;
        cmd_list->commands[cmd_idx].num_args = arg_idx;

        cmd_token = strtok_r(NULL, "|", &saveptr);
        cmd_idx++;
    }

    cmd_list->num_commands = cmd_idx;
    free(input_copy);
    return 0;
}

void free_cmd_list(cmd_list_t *cmd_list) {
    for (int i = 0; i < cmd_list->num_commands; i++) {
        for (int j = 0; j < cmd_list->commands[i].num_args; j++) {
            free(cmd_list->commands[i].args[j]);
        }
    }
    init_cmd_list(cmd_list);  // Reset structure after freeing memory
}
