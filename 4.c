
/*
    Student Record Management System
    Standard: ISO C11

    Features:
    1. Dynamic memory allocation
    2. Add student records
    3. Display all records
    4. Search by roll number
    5. Update student details
    6. Delete student records
    7. Automatic capacity growth using realloc()
    8. GPA summary (highest, lowest, average)
    9. Display records sorted by GPA
    10. Input validation and memory cleanup

    Maximum name length: 49 characters
    Valid age: 16 to 100
    Valid GPA: 0.00 to 4.00
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

#define NAME_SIZE 50
#define MAX_INITIAL_CAPACITY 1000000

/* Structure to store one student's information */
typedef struct {
    int roll_number;
    char name[NAME_SIZE];
    int age;
    float gpa;
} Student;

/* Structure to manage the dynamic student array */
typedef struct {
    Student *records;
    size_t count;
    size_t capacity;
} StudentCollection;


/* --------------------------------------------------
   INPUT FUNCTIONS
   -------------------------------------------------- */

/*
   Reads a complete line from the keyboard.
   Returns:
   1  = successful input
   0  = end of input
  -1  = input was too long
*/
int read_line(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int ch;

    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strlen(buffer);

    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
        return 1;
    }

    /* Check whether the remaining input is too long */
    ch = getchar();

    if (ch == EOF) {
        return 1;
    }

    if (ch != '\n') {
        while (ch != '\n' && ch != EOF) {
            ch = getchar();
        }
        return -1;
    }

    return 1;
}


/* Reads an integer within a specified range */
int read_integer(const char *prompt, int min, int max, int *result)
{
    char input[100];
    char *end;
    long value;
    int status;

    while (1) {
        status = read_line(prompt, input, sizeof(input));

        if (status == 0) {
            return 0;
        }

        if (status == -1) {
            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        value = strtol(input, &end, 10);

        if (input[0] == '\0' ||
            end == input ||
            *end != '\0' ||
            errno == ERANGE ||
            value < min ||
            value > max) {

            printf("Invalid input. Enter a value between %d and %d.\n",
                   min, max);
            continue;
        }

        *result = (int)value;
        return 1;
    }
}


/* Reads a GPA value between 0.00 and 4.00 */
int read_gpa(const char *prompt, float *result)
{
    char input[100];
    char *end;
    float value;
    int status;

    while (1) {
        status = read_line(prompt, input, sizeof(input));

        if (status == 0) {
            return 0;
        }

        if (status == -1) {
            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        value = strtof(input, &end);

        if (input[0] == '\0' ||
            end == input ||
            *end != '\0' ||
            errno == ERANGE ||
            !(value >= 0.0f && value <= 4.0f)) {

            printf("Invalid GPA. Enter a value from 0.00 to 4.00.\n");
            continue;
        }

        *result = value;
        return 1;
    }
}


/* Reads a student's name, including spaces */
int read_name(const char *prompt, char name[NAME_SIZE])
{
    int status;

    while (1) {
        status = read_line(prompt, name, NAME_SIZE);

        if (status == 0) {
            return 0;
        }

        if (status == -1) {
            printf("Name is too long. Maximum length is %d characters.\n",
                   NAME_SIZE - 1);
            continue;
        }

        if (name[0] == '\0') {
            printf("Name cannot be empty. Try again.\n");
            continue;
        }

        return 1;
    }
}


/* --------------------------------------------------
   COLLECTION MANAGEMENT FUNCTIONS
   -------------------------------------------------- */

/* Initializes the student collection */
int collection_init(StudentCollection *collection, size_t initial_capacity)
{
    collection->records = NULL;
    collection->count = 0;
    collection->capacity = 0;

    if (initial_capacity == 0) {
        return 1;
    }

    if (initial_capacity > SIZE_MAX / sizeof(Student)) {
        return 0;
    }

    collection->records =
        malloc(initial_capacity * sizeof(Student));

    if (collection->records == NULL) {
        return 0;
    }

    collection->capacity = initial_capacity;
    return 1;
}


/* Frees all dynamically allocated memory */
void collection_free(StudentCollection *collection)
{
    free(collection->records);

    collection->records = NULL;
    collection->count = 0;
    collection->capacity = 0;
}


/*
   Increases the capacity when the array becomes full.
   A temporary pointer is used so the original allocation
   is not lost if realloc() fails.
*/
int collection_reserve(StudentCollection *collection, size_t new_capacity)
{
    Student *temporary;

    if (new_capacity <= collection->capacity) {
        return 1;
    }

    if (new_capacity > SIZE_MAX / sizeof(Student)) {
        return 0;
    }

    temporary = realloc(
        collection->records,
        new_capacity * sizeof(Student)
    );

    if (temporary == NULL) {
        return 0;
    }

    collection->records = temporary;
    collection->capacity = new_capacity;

    return 1;
}


/* Automatically doubles the capacity when necessary */
int collection_grow(StudentCollection *collection)
{
    size_t new_capacity;

    if (collection->capacity == 0) {
        new_capacity = 2;
    } else {
        if (collection->capacity > SIZE_MAX / 2) {
            return 0;
        }

        new_capacity = collection->capacity * 2;
    }

    return collection_reserve(collection, new_capacity);
}


/* Finds a student by roll number */
int find_student(const StudentCollection *collection, int roll_number)
{
    size_t i;

    for (i = 0; i < collection->count; i++) {
        if (collection->records[i].roll_number == roll_number) {
            return (int)i;
        }
    }

    return -1;
}


/* --------------------------------------------------
   DISPLAY FUNCTIONS
   -------------------------------------------------- */

/* Displays one student's information */
void display_student(const Student *student)
{
    printf("\nRoll Number : %d\n", student->roll_number);
    printf("Name        : %s\n", student->name);
    printf("Age         : %d\n", student->age);
    printf("GPA         : %.2f\n", student->gpa);
}


/* Displays all student records */
void display_all_students(const StudentCollection *collection)
{
    size_t i;

    if (collection->count == 0) {
        printf("\nNo student records available.\n");
        return;
    }

    printf("\n========== STUDENT RECORDS ==========\n");

    for (i = 0; i < collection->count; i++) {
        printf("\nStudent %zu\n", i + 1);
        printf("-------------------------------------\n");
        display_student(&collection->records[i]);
    }

    printf("\nTotal students: %zu\n", collection->count);
    printf("Current capacity: %zu\n", collection->capacity);
}


/* --------------------------------------------------
   ADD STUDENT
   -------------------------------------------------- */

void add_student(StudentCollection *collection)
{
    Student new_student;
    int status;

    printf("\n========== ADD STUDENT ==========\n");

    if (!read_integer("Enter roll number: ",
                      1, INT_MAX, &new_student.roll_number)) {
        return;
    }

    if (find_student(collection, new_student.roll_number) != -1) {
        printf("Error: This roll number already exists.\n");
        return;
    }

    if (!read_name("Enter student name: ", new_student.name)) {
        return;
    }

    if (!read_integer("Enter age: ",
                      16, 100, &new_student.age)) {
        return;
    }

    if (!read_gpa("Enter GPA (0.00 - 4.00): ",
                  &new_student.gpa)) {
        return;
    }

    /* Grow the array if it is full */
    if (collection->count == collection->capacity) {
        status = collection_grow(collection);

        if (!status) {
            printf("Error: Memory allocation failed. "
                   "Student was not added.\n");
            return;
        }

        printf("Collection capacity increased to %zu.\n",
               collection->capacity);
    }

    collection->records[collection->count] = new_student;
    collection->count++;

    printf("\nStudent added successfully!\n");
}


/* --------------------------------------------------
   SEARCH STUDENT
   -------------------------------------------------- */

void search_student(const StudentCollection *collection)
{
    int roll_number;
    int index;

    printf("\n========== SEARCH STUDENT ==========\n");

    if (collection->count == 0) {
        printf("No records available to search.\n");
        return;
    }

    if (!read_integer("Enter roll number to search: ",
                      1, INT_MAX, &roll_number)) {
        return;
    }

    index = find_student(collection, roll_number);

    if (index == -1) {
        printf("Student with roll number %d not found.\n",
               roll_number);
    } else {
        printf("\nStudent found!\n");
        display_student(&collection->records[index]);
    }
}


/* --------------------------------------------------
   UPDATE STUDENT
   -------------------------------------------------- */

void update_student(StudentCollection *collection)
{
    int roll_number;
    int index;
    Student updated_student;

    printf("\n========== UPDATE STUDENT ==========\n");

    if (collection->count == 0) {
        printf("No records available to update.\n");
        return;
    }

    if (!read_integer("Enter roll number to update: ",
                      1, INT_MAX, &roll_number)) {
        return;
    }

    index = find_student(collection, roll_number);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    /*
       Update a temporary copy first.
       The original record remains unchanged if input ends
       before all new details are entered.
    */
    updated_student = collection->records[index];

    printf("\nEnter updated details:\n");

    if (!read_name("Enter new name: ", updated_student.name)) {
        return;
    }

    if (!read_integer("Enter new age: ",
                      16, 100, &updated_student.age)) {
        return;
    }

    if (!read_gpa("Enter new GPA (0.00 - 4.00): ",
                  &updated_student.gpa)) {
        return;
    }

    collection->records[index] = updated_student;

    printf("\nStudent record updated successfully!\n");
}


/* --------------------------------------------------
   DELETE STUDENT
   -------------------------------------------------- */

void delete_student(StudentCollection *collection)
{
    int roll_number;
    int index;
    size_t i;

    printf("\n========== DELETE STUDENT ==========\n");

    if (collection->count == 0) {
        printf("No records available to delete.\n");
        return;
    }

    if (!read_integer("Enter roll number to delete: ",
                      1, INT_MAX, &roll_number)) {
        return;
    }

    index = find_student(collection, roll_number);

    if (index == -1) {
        printf("Student not found.\n");
        return;
    }

    /* Shift all following records one position to the left */
    for (i = (size_t)index; i + 1 < collection->count; i++) {
        collection->records[i] = collection->records[i + 1];
    }

    collection->count--;

    printf("Student record deleted successfully!\n");
}


/* --------------------------------------------------
   GPA SUMMARY
   -------------------------------------------------- */

void gpa_summary(const StudentCollection *collection)
{
    size_t i;
    float highest;
    float lowest;
    float total = 0.0f;
    float average;

    printf("\n========== GPA SUMMARY ==========\n");

    if (collection->count == 0) {
        printf("No student records available for GPA summary.\n");
        return;
    }

    highest = collection->records[0].gpa;
    lowest = collection->records[0].gpa;

    for (i = 0; i < collection->count; i++) {
        float current_gpa = collection->records[i].gpa;

        if (current_gpa > highest) {
            highest = current_gpa;
        }

        if (current_gpa < lowest) {
            lowest = current_gpa;
        }

        total += current_gpa;
    }

    average = total / (float)collection->count;

    printf("Highest GPA : %.2f\n", highest);
    printf("Lowest GPA  : %.2f\n", lowest);
    printf("Average GPA : %.2f\n", average);
    printf("Total students: %zu\n", collection->count);
}


/* --------------------------------------------------
   SORTED DISPLAY
   -------------------------------------------------- */

/*
   Comparison function for qsort():

   1. Higher GPA comes first.
   2. If GPA is equal, lower roll number comes first.
*/
int compare_students(const void *a, const void *b)
{
    const Student *student_a = (const Student *)a;
    const Student *student_b = (const Student *)b;

    if (student_a->gpa > student_b->gpa) {
        return -1;
    }

    if (student_a->gpa < student_b->gpa) {
        return 1;
    }

    if (student_a->roll_number < student_b->roll_number) {
        return -1;
    }

    if (student_a->roll_number > student_b->roll_number) {
        return 1;
    }

    return 0;
}


/*
   Makes a copy of the records and sorts the copy.
   The original collection remains unchanged.
*/
void display_sorted_students(const StudentCollection *collection)
{
    Student *sorted_records;
    size_t i;

    printf("\n========== STUDENTS SORTED BY GPA ==========\n");

    if (collection->count == 0) {
        printf("No student records available.\n");
        return;
    }

    if (collection->count > SIZE_MAX / sizeof(Student)) {
        printf("Error: Collection is too large to sort.\n");
        return;
    }

    sorted_records = malloc(collection->count * sizeof(Student));

    if (sorted_records == NULL) {
        printf("Error: Unable to allocate memory for sorting.\n");
        return;
    }

    /* Copy the original records */
    memcpy(sorted_records,
           collection->records,
           collection->count * sizeof(Student));

    /* Sort the copied records */
    qsort(sorted_records,
          collection->count,
          sizeof(Student),
          compare_students);

    for (i = 0; i < collection->count; i++) {
        printf("\nRank %zu\n", i + 1);
        printf("-------------------------------------\n");
        display_student(&sorted_records[i]);
    }

    /* Free temporary memory */
    free(sorted_records);
}


/* --------------------------------------------------
   MAIN FUNCTION
   -------------------------------------------------- */

int main(void)
{
    StudentCollection collection;
    char input[100];
    char *end;
    long long initial_value;
    int choice;
    int status;

    collection.records = NULL;
    collection.count = 0;
    collection.capacity = 0;

    printf("========================================\n");
    printf("   STUDENT RECORD MANAGEMENT SYSTEM\n");
    printf("========================================\n");

    /* Ask the user for initial capacity */
    while (1) {
        status = read_line(
            "\nEnter initial capacity of student records: ",
            input,
            sizeof(input)
        );

        if (status == 0) {
            printf("\nInput ended. Exiting program.\n");
            return 0;
        }

        if (status == -1) {
            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        initial_value = strtoll(input, &end, 10);

        if (input[0] == '\0' ||
            end == input ||
            *end != '\0' ||
            errno == ERANGE ||
            initial_value < 0 ||
            initial_value > MAX_INITIAL_CAPACITY) {

            printf("Invalid capacity. Enter a number from 0 to %d.\n",
                   MAX_INITIAL_CAPACITY);
            continue;
        }

        break;
    }

    if (!collection_init(&collection, (size_t)initial_value)) {
        printf("Error: Initial memory allocation failed.\n");
        return 1;
    }

    printf("\nCollection initialized successfully.\n");
    printf("Initial capacity: %zu\n", collection.capacity);

    /* Main menu loop */
    while (1) {

        printf("\n\n========================================\n");
        printf("                 MAIN MENU\n");
        printf("========================================\n");

        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. GPA Summary\n");
        printf("7. Display Students Sorted by GPA\n");
        printf("8. Display Collection Information\n");
        printf("0. Exit\n");

        printf("========================================\n");

        if (!read_integer("Enter your choice: ", 0, 8, &choice)) {
            printf("\nInput ended. Exiting program.\n");
            break;
        }

        switch (choice) {

            case 1:
                add_student(&collection);
                break;

            case 2:
                display_all_students(&collection);
                break;

            case 3:
                search_student(&collection);
                break;

            case 4:
                update_student(&collection);
                break;

            case 5:
                delete_student(&collection);
                break;

            case 6:
                gpa_summary(&collection);
                break;

            case 7:
                display_sorted_students(&collection);
                break;

            case 8:
                printf("\n========== COLLECTION INFORMATION ==========\n");
                printf("Total records : %zu\n", collection.count);
                printf("Capacity      : %zu\n", collection.capacity);
                printf("Available slots: %zu\n",
                       collection.capacity - collection.count);
                break;

            case 0:
                printf("\nExiting Student Record Management System...\n");
                goto cleanup;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

cleanup:

    /* Release all dynamically allocated memory */
    collection_free(&collection);

    printf("All allocated memory has been released.\n");
    printf("Program terminated successfully.\n");

    return 0;
}
