#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdbsc.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [option] ...\n", argv[0]);
        return 1;
    }

    char *command = argv[1];

    // Option parsing
    if (strcmp(argv[1], "-a") == 0) {
        // Add a new student
        if (argc != 6) {
            fprintf(stderr, "Usage: %s -a <id> <fname> <lname> <gpa>\n", argv[0]);
            return 1;
        }

        int id = atoi(argv[2]);
        const char *fname = argv[3];
        const char *lname = argv[4];
        int gpa = atoi(argv[5]);

        if (add_student(id, fname, lname, gpa) == 0) {
            printf("Student %d added to database.\n", id);
        } else {
            printf("Failed to add student %d to database.\n", id);
        }
    }
    else if (strcmp(command, "-p") == 0) {
        // Print all students
        print_all_students();
    }
    else if (strcmp(command, "-c") == 0) {
        // Count the number of students
        int count = count_students();
        if (count >= 0) {
            printf("Database contains %d student record(s).\n", count);
        } else {
            printf("Error counting records.\n");
        }
    }
    else if (strcmp(argv[1], "-x") == 0) {
        // Compress the database
        compress_db();
        printf("Database successfully compressed!\n");
    }
    else if (strcmp(argv[1], "-d") == 0) {
    // Delete a student
    if (argc != 3) {
        fprintf(stderr, "Usage: %s -d <id>\n", argv[0]);
        return 1;
    }

    int id = atoi(argv[2]);
    if (delete_student(id) == 0) {
        printf("Student %d was deleted from database.\n", id);
    } else {
        printf("Student %d was not found in database.\n", id);
    }
    }
    else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s -f <id>\n", argv[0]);
            return 1;
        }

        int id = atoi(argv[2]);
        print_student(id);
    }

    return 0;
}
