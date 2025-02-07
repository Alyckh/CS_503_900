#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sdbsc.h"

int add_student(int id, const char *fname, const char *lname, int gpa) {
    // check if the student ID already exists
    if (find_student(id) != NULL) {
        printf("Cant add student with ID=%d, already exists in db.\n", id);
        return 1;  // failure code if the student already exists
    }

    int fd = open(DB_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening database file");
        return -1;  // error opening file
    }

    student_t new_student = {id, "", "", gpa};  // new student record
    strncpy(new_student.fname, fname, sizeof(new_student.fname) - 1);
    strncpy(new_student.lname, lname, sizeof(new_student.lname) - 1);
    new_student.fname[sizeof(new_student.fname) - 1] = '\0';
    new_student.lname[sizeof(new_student.lname) - 1] = '\0';

    off_t offset = (id - 1) * sizeof(student_t);
    lseek(fd, offset, SEEK_SET);

    if (write(fd, &new_student, sizeof(student_t)) != sizeof(student_t)) {
        perror("Error writing to database file");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Function to compress the database by removing deleted students
void compress_db() {
    int fd = open(DB_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening database file");
        return;
    }

    student_t student;
    int valid_count = 0;

    // Process all students in the database
    while (read(fd, &student, sizeof(student_t)) == sizeof(student_t)) {
        if (student.id != 0) {
            lseek(fd, valid_count * sizeof(student_t), SEEK_SET);
            write(fd, &student, sizeof(student_t));
            valid_count++;
        }
    }

    // Truncate the file to remove gaps from deleted students
    ftruncate(fd, valid_count * sizeof(student_t));

    close(fd);  // Close the file after processing
}

void print_student(int id) {
    if (id < MIN_STD_ID || id > MAX_STD_ID) {
        fprintf(stderr, "Invalid student ID.\n");
        return;
    }

    int fd = open(DB_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Error opening database file");
        return;
    }

    student_t student;
    off_t offset = (id - 1) * sizeof(student_t);
    lseek(fd, offset, SEEK_SET);

    if (read(fd, &student, sizeof(student_t)) == sizeof(student_t) && student.id != 0) {
        printf("ID: %d, Name: %s %s, GPA: %.2f\n", student.id, student.fname, student.lname, student.gpa / 100.0);
    } else {
        printf("Student ID %d not found.\n", id);
    }

    close(fd);
}

void print_all_students() {
    int fd = open(DB_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Error opening database file");
        return;
    }

    student_t student;
    int printed_count = 0;

    // Move the file pointer to the start of the file
    lseek(fd, 0, SEEK_SET);

    // Read all records and print only valid ones (id != 0)
    while (read(fd, &student, sizeof(student_t)) == sizeof(student_t)) {
        if (student.id != 0) {  // Check if the student ID is valid
            printf("ID: %d, Name: %s %s, GPA: %.2f\n", student.id, student.fname, student.lname, student.gpa / 100.0);
            printed_count++;
        }
    }

    if (printed_count == 0) {
        printf("No valid student records found.\n");
    }

    close(fd);
}

student_t* find_student(int id) {
    static student_t student;

    int fd = open(DB_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Error opening database file");
        return NULL;
    }

    off_t offset = (id - 1) * sizeof(student_t);
    lseek(fd, offset, SEEK_SET);

    if (read(fd, &student, sizeof(student_t)) == sizeof(student_t) && student.id != 0) {
        close(fd);
        return &student;  // Return pointer to the static student
    }

    close(fd);
    return NULL;  // Return NULL if student not found
}

// Count students
int count_students() {
    int fd = open(DB_FILE, O_RDONLY);
    if (fd == -1) {
        perror("Error opening database file");
        return -1;
    }

    student_t student;
    int valid_count = 0;

    // Read each student record and count only valid ones
    while (read(fd, &student, sizeof(student_t)) == sizeof(student_t)) {
        if (student.id != 0) {  // Only count valid student records
            valid_count++;
        }
    }

    close(fd);
    return valid_count;
}

int delete_student(int id) {
    int fd = open(DB_FILE, O_RDWR);
    if (fd == -1) {
        perror("Error opening database file");
        return -1;
    }

    // Calculate the offset for the student record based on their ID
    off_t offset = (id - 1) * sizeof(student_t);
    lseek(fd, offset, SEEK_SET);

    student_t student;
    if (read(fd, &student, sizeof(student_t)) != sizeof(student_t)) {
        close(fd);
        fprintf(stderr, "Student %d not found in database.\n", id);
        return -1;
    }

    // If student ID is valid mark as deleted
    if (student.id != 0) {
        student.id = 0;
        lseek(fd, offset, SEEK_SET);
        write(fd, &student, sizeof(student_t));
        close(fd);
        return 0;
    }

    // If student ID is 0 close and return error
    close(fd);
    return -1;
}
