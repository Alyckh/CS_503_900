#ifndef SDBSC_H
#define SDBSC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DB_FILE "student.db"
#define RECORD_SIZE sizeof(student_t)

// Student structure
typedef struct {
    int id;             // Student ID (unique key)
    char name[50];      // Student name
    float gpa;         // GPA
} student_t;

// Function declarations
int add_student(int id, const char *name, float gpa);
int delete_student(int id);
student_t *find_student(int id);
int update_student(int id, const char *name, float gpa);
void list_students();
int compress_db(); // Extra credit

#endif // SDBSC_H
