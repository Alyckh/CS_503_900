#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SZ 50

// Function prototypes
void setup_buff(char* buff, const char* user_str, size_t buff_sz);
int count_words(const char* buff, size_t len);
void reverse_string(const char* buff, size_t len);
void word_print(const char* buff, size_t len);
void print_buff(const char* buff, size_t len);

void error_exit(const char* msg, int code) {
    fprintf(stderr, "%s\n", msg);
    exit(code);
}

// Function to setup the buffer
void setup_buff(char* buff, const char* user_str, size_t buff_sz) {
    size_t i = 0, j = 0;
    int prev_space = 0;

    while (user_str[i] != '\0' && j < buff_sz) {
        if (isspace(user_str[i])) {
            if (!prev_space && j < buff_sz - 1) {
                buff[j++] = ' ';
                prev_space = 1;
            }
        }
        else {
            buff[j++] = user_str[i];
            prev_space = 0;
        }
        i++;
    }

    if (user_str[i] != '\0') {
        error_exit("error: Provided input string is too long", 3);
    }

    while (j < buff_sz) {
        buff[j++] = '.';
    }
}

// Function to count words
int count_words(const char* buff, size_t len) {
    int count = 0;
    int in_word = 0;

    for (size_t i = 0; i < len; i++) {
        if (buff[i] == '.') break;

        if (!isspace(buff[i]) && !in_word) {
            in_word = 1;
            count++;
        }
        else if (isspace(buff[i])) {
            in_word = 0;
        }
    }

    return count;
}

// Function to reverse string
void reverse_string(const char* buff, size_t len) {
    const char* end = buff + len - 1;
    while (*end == '.') end--;

    for (; end >= buff; end--) {
        putchar(*end);
    }
    putchar('\n');
}

// Function to print words with their lengths
void word_print(const char* buff, size_t len) {
    int word_index = 1;
    int word_len = 0;

    printf("Word Print\n----------\n");
    for (size_t i = 0; i < len; i++) {
        if (buff[i] == '.' || buff[i] == '\0') break;

        if (!isspace(buff[i])) {
            putchar(buff[i]);
            word_len++;
        }
        else if (word_len > 0) {
            printf(" (%d)\n", word_len);
            word_index++;
            word_len = 0;
        }
    }

    if (word_len > 0) {
        printf(" (%d)\n", word_len);
    }
}

// Function to print the buffer
void print_buff(const char* buff, size_t len) {
    printf("Buffer: \"%.*s\"\n", (int)len, buff);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        error_exit("Usage: ./stringfun -<option> <string>", 1);
    }

    char option = argv[1][1];
    const char* user_str = argv[2];

    if (strlen(user_str) > BUFFER_SZ) {
        error_exit("error: Input string exceeds buffer size.", 3);
    }

    char* buff = malloc(BUFFER_SZ);
    if (!buff) {
        error_exit("error: Memory allocation failure.", 2);
    }

    setup_buff(buff, user_str, BUFFER_SZ);
    size_t len = BUFFER_SZ;

    switch (option) {
    case 'c': {
        int word_count = count_words(buff, len);
        printf("Word Count: %d\n", word_count);
        break;
    }
    case 'r': {
        printf("Reversed String: ");
        reverse_string(buff, len);
        break;
    }
    case 'w': {
        word_print(buff, len);
        break;
    }
    case 'x': {
        if (argc != 5) {
            error_exit("error: -x option requires 3 arguments", 1);
        }

        printf("Not Implemented!\n");
        exit(3);
    }
    default:
        error_exit("error: Unknown option", 1);
    }

    print_buff(buff, len);
    free(buff);
    return 0;
}
