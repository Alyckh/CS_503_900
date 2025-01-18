#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//Constants
#define SPACE_CHAR ' '

//Function Prototypes
void usage(char* exename);
int count_words(char* str);
void reverse_string(char* str);
void word_print(char* str);

//Displays usage information for the program
void usage(char* exename) {
    printf("usage: %s [-h|c|r|w] \"string\"\n", exename);
    printf("\texample: %s -w \"hello class\"\n", exename);
}

//Counts the number of words in a given string
int count_words(char* str) {
    int len = strlen(str);
    int wc = 0;
    bool word_start = false;

    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (!word_start) {
            if (c != SPACE_CHAR) {
                wc++;
                word_start = true;
            }
        }
        else {
            if (c == SPACE_CHAR) {
                word_start = false;
            }
        }
    }
    return wc;
}

//TODO #1: Why provide function prototypes?
    // Function prototypes ensure that the compiler knows about the functions' signatures before their actual definitions. This allows for easier changes to the program and proper type checking of function calls.

//Reverses the given string in place
void reverse_string(char* str) {
    int start_idx = 0;
    int end_idx = strlen(str) - 1;
    while (start_idx < end_idx) {
        char tmp = str[start_idx];
        str[start_idx] = str[end_idx];
        str[end_idx] = tmp;
        start_idx++;
        end_idx--;
    }
}

//Prints each word in teh string with its index and length
void word_print(char* str) {
    int len = strlen(str);
    int last_char_idx = len - 1;
    int wc = 0;
    int wlen = 0;
    bool word_start = false;

    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (!word_start) {
            if (c != SPACE_CHAR) {
                wc++;
                word_start = true;
                wlen = 0;
                printf("%d. ", wc);
            }
        }
        if (word_start) {
            if (c != SPACE_CHAR) {
                printf("%c", c);
                wlen++;
            }
            if (c == SPACE_CHAR || i == last_char_idx) {
                printf(" (%d)\n", wlen);
                word_start = false;
            }
        }
    }
}

//Main Function
int main(int argc, char* argv[]) {
    char* input_string;
    char* opt_string;
    char opt;

    //Validate the number of arguments
    if (argc < 2) {
        usage(argv[0]);
        exit(1);
    }

    opt_string = argv[1];

    //Validate the option format
    if ((opt_string[0] != '-') || (strlen(opt_string) != 2)) {
        usage(argv[0]);
        exit(1);
    }

    opt = opt_string[1];

    //Handle help option
    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    //Ensure input string is provided
    if (argc != 3) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2];

    //TODO #2: Call count_words, return of the result should go into the wc variable. (-c "Now count these words")
        //Word Count: 4
    switch (opt) {
    case 'c': {
        int wc = count_words(input_string);
        printf("Word Count: %d\n", wc);
        break;
    }

    //TODO #3: Call reverse string using input_string, inpute string should be reversed. (-r "This will be in reverse now")
        //Reversed string: won esrever ni eb lliw sihT
    case 'r': {
        reverse_string(input_string);
        printf("Reversed string: %s\n", input_string);
        break;
    }

    //TODO #4: Why is the string reversed in place?
        //In-place reversal modifies the original string directly in memory. This makes the process more efficient.


    case 'w': {
        printf("Word Print\n----------\n");
        word_print(input_string);
        break;
    }
    //TODO #5: Call word_print, output should be printed by that function (-w "This is the output")
        //1. This (4)
        //2. is(2)
        //3. the(3)
        //4. output(6)

    default:
        usage(argv[0]);
        printf("Invalid option %c provided, exiting!\n", opt);
        exit(1);
    }

    //TODO: #6 What is the purpose of the default option?
        //The default case handles invalid or unsupported options. It ensures the program can inform the user of the error and exits instead of behaving oddly.

    //TODO: #7 Why use break in each case but not default?
        //The break statement prevents fall-through in switch cases. Without it, the program would continue into the next case. The default case doesn't require a break because it's the last one, and the program exits immediately after handling it.

    return 0;
}
