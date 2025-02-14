#ifndef DSHLIB_H
#define DSHLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TOKENS 128
#define MAX_COMMANDS 8

typedef struct {
    char *args[MAX_TOKENS];  // Array of arguments
    int num_args;            // Number of arguments
} command_t;

typedef struct {
    command_t commands[MAX_COMMANDS];  // Array of parsed commands
    int num_commands;                  // Number of commands
} cmd_list_t;

void init_cmd_list(cmd_list_t *cmd_list);
int build_cmd_list(cmd_list_t *cmd_list, const char *input);
void free_cmd_list(cmd_list_t *cmd_list);

#endif // DSHLIB_H
