#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SZ 50

// Function prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);
int count_words(char *, int, int);
int reverse_string(char *, int);
int print_words_and_lengths(char *, int);
int replace_word(char *, int, char *, char *);

// Function to setup the buffer based on input string
int setup_buff(char *buff, char *user_str, int len) {
    char *src = user_str; // Pointer to traverse user string
    char *dest = buff;    // Pointer to traverse buffer
    int count = 0;        // Counts non-whitespace characters

    // Iterate through user string
    while (*src != '\0') {
        if (*src == ' ' || *src == '\t') { // Handle whitespace
            if (dest != buff && *(dest - 1) != ' ') {
                *dest++ = ' ';
                count++;
            }
        } else {
            *dest++ = *src;
            count++;
        }
        src++;
        if (count >= len) return -1; // Buffer overflow
    }

    // Fill remaining buffer with dots
    while (count < len) {
        *dest++ = '.';
        count++;
    }

    return count;
}

// Function to count words
int count_words(char *buff, int len, int str_len) {
    int count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '.') {
            if (!in_word) {
                in_word = 1;
                count++;
            }
        } else {
            in_word = 0;
        }
    }

    return count;
}

// Function to reverse the string
int reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;
    char temp;

    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    return 0;
}

// Function to print words and their lengths
int print_words_and_lengths(char *buff, int str_len) {
    printf("Word Print\n----------\n");
    char *start = buff;
    int word_len = 0;
    int word_count = 1;

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '.') {
            if (word_len == 0) {
                printf("%d. ", word_count++);
                start = buff + i;
            }
            putchar(*(buff + i));
            word_len++;
        } else if (word_len > 0) {
            printf(" (%d)\n", word_len);
            word_len = 0;
        }
    }

    if (word_len > 0) {
        printf(" (%d)\n", word_len);
    }

    return 0;
}

// Function to replace a word
int replace_word(char *buff, int len, char *search, char *replace) {
    char *found = NULL;
    char *end = buff + len;
    int search_len = 0, replace_len = 0;

    // Calculate lengths of search and replace strings
    while (search[search_len] != '\0') search_len++;
    while (replace[replace_len] != '\0') replace_len++;

    // Search for the word
    for (char *ptr = buff; ptr < end - search_len; ptr++) {
        if (*ptr == *search && strncmp(ptr, search, search_len) == 0) {
            found = ptr;
            break;
        }
    }

    if (found) {
        if (replace_len > search_len && (end - buff) < len - (replace_len - search_len)) {
            return -1; // Buffer overflow
        }

        // Move characters to adjust for the length difference
        if (replace_len != search_len) {
            memmove(found + replace_len, found + search_len, (end - found - search_len));
        }

        // Copy replacement string
        for (int i = 0; i < replace_len; i++) {
            *(found + i) = *(replace + i);
        }
    }

    return 0;
}

// Function to print the buffer
void print_buff(char *buff, int len) {
    printf("Buffer:  ");
    for (int i = 0; i < len; i++) {
        putchar(*(buff + i));
    }
    putchar('\n');
}

// Function to print usage information
void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

// Main function
int main(int argc, char *argv[]) {
    char *buff;             // Internal buffer
    char *input_string;     // User input string
    char opt;               // Option flag
    int rc;                 // Return code
    int user_str_len;       // User string length

    // TODO #1: Why is this safe?
    // This is safe because it will check if the number of arguments is less than 2 (`argc < 2`) before attempting to access `argv[1]`.
    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char) *(argv[1] + 1); // Get option flag

    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    // TODO #2: Document the purpose of the if statement below.
    // This checks if the user provided an input string (`argc < 3`). If not, the program exits.
    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2];

    // TODO #3: Allocate buffer and handle malloc failure
    buff = (char *) malloc(BUFFER_SZ);
    if (!buff) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0) {
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt) {
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: ");
            for (int i = 0; i < user_str_len; i++) {
                putchar(*(buff + i));
            }
            putchar('\n');
            break;
        case 'w':
            print_words_and_lengths(buff, user_str_len);
            break;
        case 'x':
            if (argc < 5) {
                printf("Error: Missing arguments for replace\n");
                free(buff);
                exit(1);
            }
            rc = replace_word(buff, BUFFER_SZ, argv[3], argv[4]);
            if (rc < 0) {
                printf("Error: Buffer overflow during replace\n");
                free(buff);
                exit(2);
            }
            printf("Modified String: ");
            for (int i = 0; i < BUFFER_SZ && buff[i] != '.'; i++) {
                putchar(buff[i]);
            }
            putchar('\n');
            break;
        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    // TODO #6: Free the buffer before exiting
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}

// TODO #7: Why is providing both the pointer and length good practice?
// Providing both the pointer and the length ensures the program knows the size of the buffer, which prevents accidental overflows.
