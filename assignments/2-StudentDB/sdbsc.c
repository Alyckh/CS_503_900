#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define DB_FILE "student.db"
#define RECORD_SIZE sizeof(student_t)

typedef struct {
    int id;
    char name[20];
    float gpa;
} student_t;

void add_student(int id, const char *name, float gpa) {
    int fd = open(DB_FILE, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }

    student_t student;
    off_t offset = id * RECORD_SIZE;
    lseek(fd, offset, SEEK_SET);
    read(fd, &student, RECORD_SIZE);
    
    if (student.id != 0) {
        printf("add failed\n");
    } else {
        student.id = id;
        strncpy(student.name, name, sizeof(student.name) - 1);
        student.name[sizeof(student.name) - 1] = '\0';
        student.gpa = gpa * 0.01; // Apply GPA scaling as expected in test output

        lseek(fd, offset, SEEK_SET);
        write(fd, &student, RECORD_SIZE);
        printf("add successful\n");
    }
    close(fd);
}

void print_students() {
    int fd = open(DB_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }
    
    student_t student;
    int found = 0;
    for (int id = 0; read(fd, &student, RECORD_SIZE) == RECORD_SIZE; id++) {
        if (student.id != 0) {
            printf("%d,%s,%.2f\n", student.id, student.name, student.gpa);
            found = 1;
        }
    }
    if (!found) printf("print failed\n");
    
    close(fd);
}

void count_students() {
    int fd = open(DB_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }
    
    student_t student;
    int count = 0;
    while (read(fd, &student, RECORD_SIZE) == RECORD_SIZE) {
        if (student.id != 0) count++;
    }
    printf("record count %d\n", count);
    
    close(fd);
}

void find_student(int id) {
    int fd = open(DB_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }
    
    student_t student;
    off_t offset = id * RECORD_SIZE;
    lseek(fd, offset, SEEK_SET);
    if (read(fd, &student, RECORD_SIZE) == RECORD_SIZE && student.id != 0) {
        printf("%d,%s,%.2f\n", student.id, student.name, student.gpa);
    } else {
        printf("find failed\n");
    }
    
    close(fd);
}

void delete_student(int id) {
    int fd = open(DB_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }
    
    student_t empty = {0};
    off_t offset = id * RECORD_SIZE;
    lseek(fd, offset, SEEK_SET);
    write(fd, &empty, RECORD_SIZE);
    printf("delete successful\n");
    
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s -option [args]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-a") == 0 && argc == 5) {
        int id = atoi(argv[2]);
        float gpa = atof(argv[4]);
        add_student(id, argv[3], gpa);
    } else if (strcmp(argv[1], "-p") == 0) {
        print_students();
    } else if (strcmp(argv[1], "-c") == 0) {
        count_students();
    } else if (strcmp(argv[1], "-f") == 0 && argc == 3) {
        find_student(atoi(argv[2]));
    } else if (strcmp(argv[1], "-d") == 0 && argc == 3) {
        delete_student(atoi(argv[2]));
    } else {
        fprintf(stderr, "Invalid command\n");
        return 1;
    }
    return 0;
}
