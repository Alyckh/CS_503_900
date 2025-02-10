#include "dshlib.h"

void print_prompt() {
    printf("dsh> ");
    fflush(stdout);
}

void read_command(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        printf("\n");
        exit(0);
    }
}

int main() {
    char input[1024];
    cmd_list_t cmd_list;

    while (1) {
        print_prompt();
        read_command(input, sizeof(input));

        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Handle empty inputs
        if (strlen(input) == 0) {
            printf("warning: no commands provided\n");
            continue;
        }

        // Handle exit command
        if (strcmp(input, "exit") == 0) {
            break;
        }

        init_cmd_list(&cmd_list);
        if (build_cmd_list(&cmd_list, input) == 0) {
            printf("PARSEDCOMMANDLINE-TOTALCOMMANDS %d ", cmd_list.num_commands);
            for (int i = 0; i < cmd_list.num_commands; i++) {
                printf("<%d> %s", i + 1, cmd_list.commands[i].args[0]);

                if (cmd_list.commands[i].num_args > 1) {
                    printf(" [");
                    for (int j = 1; j < cmd_list.commands[i].num_args; j++) {
                        if (j > 1) printf(" ");
                        printf("%s", cmd_list.commands[i].args[j]);
                    }
                    printf("]");
                }
                printf(" ");
            }
            printf("\n");
        }

        free_cmd_list(&cmd_list);
    }

    return 0;
}
