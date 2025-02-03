#ifndef SDBSC_H
#define SDBSC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DB_FILE "student.db"  // Path to the database file
#define MAX_STD_ID 1000  // Example maximum student ID
#define MIN_STD_ID 1  // Example minimum student ID

// Structure for storing student data
typedef struct {
    int id;
    char fname[100];
    char lname[100];
    int gpa;  // GPA is stored as an integer (e.g., 3500 for 3.5 GPA)
} student_t;

// Function declarations
int add_student(int id, const char *fname, const char *lname, int gpa);
void print_student(int id);  // Print a student record by ID
student_t* find_student(int id);  // Find a student by ID
int count_students(void);  // Count the number of students in the database
void print_all_students(void);  // Print all student records
int delete_student(int id);  // Delete a student record
void compress_db(void);  // Remove deleted records and compress the database

#endif  // SDBSC_H
