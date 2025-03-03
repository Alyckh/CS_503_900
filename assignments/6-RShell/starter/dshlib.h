#ifndef __DSHLIB_H__
#define __DSHLIB_H__

typedef struct cmd_buff cmd_buff_t;
typedef struct command_list command_list_t;

#include <stdbool.h>
#include "rshlib.h"

#define EXE_MAX 64
#define ARG_MAX 256
#define CMD_MAX 8
#define CMD_ARGV_MAX (CMD_MAX + 1)
#define SH_CMD_MAX (EXE_MAX + ARG_MAX)

typedef struct cmd_buff {
    int argc;
    char *argv[CMD_ARGV_MAX];
    char *_cmd_buffer;
} cmd_buff_t;

typedef struct command_list {
    int num_commands;
    cmd_buff_t commands[CMD_MAX];
} command_list_t; 

#define SPACE_CHAR  ' '
#define PIPE_CHAR   '|'
#define PIPE_STRING "|"

#define SH_PROMPT "dsh4> "
#define EXIT_CMD "exit"

// Standard Return Codes
#define OK                       0
#define WARN_NO_CMDS            -1
#define ERR_TOO_MANY_COMMANDS   -2
#define ERR_CMD_OR_ARGS_TOO_BIG -3
#define ERR_MEMORY              -5
#define ERR_EXEC_CMD            1
#define OK_EXIT                 0

#define CMD_WARN_NO_CMD "warning: no commands provided\n"

// Function declarations for cmd_buff
int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);

// Function declarations for command_list
command_list_t *build_cmd_list(char *cmd);
void free_command_list(command_list_t *clist);

#endif
