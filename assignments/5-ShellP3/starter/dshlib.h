#ifndef __DSHLIB_H__
#define __DSHLIB_H__

#include <stdbool.h>

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

#define SPACE_CHAR  ' '
#define PIPE_CHAR   '|'
#define PIPE_STRING "|"

#define SH_PROMPT "dsh3> "
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

int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);

typedef enum {
    BI_CMD_EXIT,
    BI_CMD_CD,
    BI_NOT_BI,
    BI_EXECUTED,
} Built_In_Cmds;

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd);
int exec_local_cmd_loop();
bool is_valid_command(const char *cmd);

#endif
