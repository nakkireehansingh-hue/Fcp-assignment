#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_CAPACITY_LIMIT 1000
#define MAX_DESC_LEN 256
#define MAX_LINE_LEN 512

// Task status enumeration
typedef enum {
    STATUS_PENDING,
    STATUS_DONE
} TaskStatus;

// Command enumeration
typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

// Task structure
typedef struct {
    int id;
    char description[MAX_DESC_LEN];
    TaskStatus status;
} Task;

// Function prototypes
CommandType parse_command(const char *cmd_str);
int find_index_by_id(Task *tasks, int count, int id);
void add_task(Task **tasks, int *count, int *capacity, int id, const char *description);
void delete_task(Task *tasks, int *count, int id);
void update_task(Task *tasks, int count, int id, const char *new_desc);
void complete_task(Task *tasks, int count, int id);
void view_tasks(Task *tasks, int count);
void find_task(Task *tasks, int count, int id);
void print_help(void);
void trim_whitespace(char *str);
bool read_line(char *buffer, int size);

// Case-insensitive string comparison helper
int string_case_cmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// Convert command string to enum
CommandType parse_command(const char *cmd_str) {
    if (string_case_cmp(cmd_str, "ADD") == 0) return CMD_ADD;
    if (string_case_cmp(cmd_str, "DELETE") == 0) return CMD_DELETE;
    if (string_case_cmp(cmd_str, "VIEW") == 0) return CMD_VIEW;
    if (string_case_cmp(cmd_str, "UPDATE") == 0) return CMD_UPDATE;
    if (string_case_cmp(cmd_str, "COMPLETE") == 0) return CMD_COMPLETE;
    if (string_case_cmp(cmd_str, "FIND") == 0) return CMD_FIND;
    if (string_case_cmp(cmd_str, "HELP") == 0) return CMD_HELP;
    if (string_case_cmp(cmd_str, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

// Trim leading and trailing whitespace / newlines
void trim_whitespace(char *str) {
    if (!str) return;
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || isspace((unsigned char)str[len - 1]))) {
        str[--len] = '\0';
    }
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

// Bounded line reader with overlong line detection & flushing
bool read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return false; // EOF or read error
    }
    // Check if line was too long (no newline found within buffer)
    if (strchr(buffer, '\n') == NULL) {
        int c;
        // Flush remaining characters in the input stream
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Error: Input line exceeded maximum allowed length (%d characters).\n", size - 1);
        buffer[0] = '\0';
        return false;
    }
    trim_whitespace(buffer);
    return true;
}

// Search helper returning index or -1
int find_index_by_id(Task *tasks, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Add a task with dynamic capacity resizing (realloc)
void add_task(Task **tasks, int *count, int *capacity, int id, const char *description) {
    if (id <= 0) {
        printf("Error: Task ID must be a positive integer.\n");
        return;
    }

    if (find_index_by_id(*tasks, *count, id) != -1) {
        printf("Error: Task with ID %d already exists.\n", id);
        return;
    }

    if (strlen(description) == 0) {
        printf("Error: Task description cannot be empty.\n");
        return;
    }

    // Check capacity expansion
    if (*count >= *capacity) {
        int new_capacity = *capacity * 2;
        if (new_capacity > MAX_CAPACITY_LIMIT) {
            new_capacity = MAX_CAPACITY_LIMIT;
        }
        if (*count >= new_capacity) {
            printf("Error: Maximum capacity limit (%d) reached.\n", MAX_CAPACITY_LIMIT);
            return;
        }

        Task *temp = (Task *)realloc(*tasks, new_capacity * sizeof(Task));
        if (temp == NULL) {
            printf("Error: Memory reallocation failed. Could not expand capacity.\n");
            return;
        }
        *tasks = temp;
        *capacity = new_capacity;
    }

    // Insert task
    (*tasks)[*count].id = id;
    snprintf((*tasks)[*count].description, MAX_DESC_LEN, "%s", description);
    (*tasks)[*count].status = STATUS_PENDING;
    (*count)++;

    printf("Success: Task ID %d created successfully.\n", id);
}

// Delete task by ID and close gap
void delete_task(Task *tasks, int *count, int id) {
    int idx = find_index_by_id(tasks, *count, id);
    if (idx == -1) {
        printf("Error: Task with ID %d not found for deletion.\n", id);
        return;
    }

    for (int i = idx; i < *count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    (*count)--;
    printf("Success: Task ID %d deleted successfully.\n", id);
}

// Update task description
void update_task(Task *tasks, int count, int id, const char *new_desc) {
    int idx = find_index_by_id(tasks, count, id);
    if (idx == -1) {
        printf("Error: Task with ID %d not found for update.\n", id);
        return;
    }

    if (strlen(new_desc) == 0) {
        printf("Error: New task description cannot be empty.\n");
        return;
    }

    snprintf(tasks[idx].description, MAX_DESC_LEN, "%s", new_desc);
    printf("Success: Task ID %d updated successfully.\n", id);
}

// Mark task as DONE
void complete_task(Task *tasks, int count, int id) {
    int idx = find_index_by_id(tasks, count, id);
    if (idx == -1) {
        printf("Error: Task with ID %d not found.\n", id);
        return;
    }

    tasks[idx].status = STATUS_DONE;
    printf("Success: Task ID %d marked as DONE.\n", id);
}

// View all tasks
void view_tasks(Task *tasks, int count) {
    if (count == 0) {
        printf("\n[Task list is empty]\n");
        return;
    }

    printf("\n=========================================================\n");
    printf("                      TASK LIST                          \n");
    printf("=========================================================\n");
    for (int i = 0; i < count; i++) {
        printf("ID: %-5d | Status: %-8s | Description: %s\n",
               tasks[i].id,
               tasks[i].status == STATUS_DONE ? "DONE" : "PENDING",
               tasks[i].description);
    }
    printf("=========================================================\n");
}

// Find and display a single task by ID
void find_task(Task *tasks, int count, int id) {
    int idx = find_index_by_id(tasks, count, id);
    if (idx == -1) {
        printf("Search Result: Task with ID %d not found.\n", id);
        return;
    }

    printf("\n--- Task ID %d ---\n", id);
    printf("Status      : %s\n", tasks[idx].status == STATUS_DONE ? "DONE" : "PENDING");
    printf("Description : %s\n", tasks[idx].description);
}

// Display help menu
void print_help(void) {
    printf("\n==================== AVAILABLE COMMANDS ====================\n");
    printf("  ADD      - Create a new task (prompts for ID and description)\n");
    printf("  DELETE   - Remove a task by ID\n");
    printf("  VIEW     - List all tasks with status and description\n");
    printf("  UPDATE   - Modify an existing task's description\n");
    printf("  COMPLETE - Mark a task as DONE\n");
    printf("  FIND     - Retrieve a specific task by ID\n");
    printf("  HELP     - Display this help menu\n");
    printf("  EXIT     - Terminate the program and free memory\n");
    printf("============================================================\n");
}

int main() {
    int initial_capacity = 2;
    int count = 0;

    Task *tasks = (Task *)malloc(initial_capacity * sizeof(Task));
    if (tasks == NULL) {
        printf("Initial memory allocation failed.\n");
        return 1;
    }

    char line_buffer[MAX_LINE_LEN];

    printf("=== Task Management System Initialized ===\n");
    printf("Type 'HELP' for a list of valid commands.\n");

    while (true) {
        printf("\n> ");
        if (!read_line(line_buffer, sizeof(line_buffer))) {
            if (feof(stdin)) {
                printf("\nEnd of input (EOF) detected. Exiting...\n");
                break;
            }
            continue;
        }

        if (strlen(line_buffer) == 0) {
            continue;
        }

        // Extract command token
        char cmd_str[32];
        int scanned = sscanf(line_buffer, "%31s", cmd_str);
        if (scanned != 1) {
            continue;
        }

        CommandType cmd = parse_command(cmd_str);

        switch (cmd) {
            case CMD_ADD: {
                int id;
                printf("Enter Task ID (positive integer): ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                if (sscanf(line_buffer, "%d", &id) != 1) {
                    printf("Error: Invalid numeric ID format.\n");
                    break;
                }

                printf("Enter Description: ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                add_task(&tasks, &count, &initial_capacity, id, line_buffer);
                break;
            }
            case CMD_DELETE: {
                int id;
                printf("Enter Task ID to delete: ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                if (sscanf(line_buffer, "%d", &id) != 1) {
                    printf("Error: Invalid numeric ID format.\n");
                    break;
                }
                delete_task(tasks, &count, id);
                break;
            }
            case CMD_VIEW:
                view_tasks(tasks, count);
                break;
            case CMD_UPDATE: {
                int id;
                printf("Enter Task ID to update: ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                if (sscanf(line_buffer, "%d", &id) != 1) {
                    printf("Error: Invalid numeric ID format.\n");
                    break;
                }

                printf("Enter New Description: ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                update_task(tasks, count, id, line_buffer);
                break;
            }
            case CMD_COMPLETE: {
                int id;
                printf("Enter Task ID to complete: ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                if (sscanf(line_buffer, "%d", &id) != 1) {
                    printf("Error: Invalid numeric ID format.\n");
                    break;
                }
                complete_task(tasks, count, id);
                break;
            }
            case CMD_FIND: {
                int id;
                printf("Enter Task ID to search: ");
                if (!read_line(line_buffer, sizeof(line_buffer))) break;
                if (sscanf(line_buffer, "%d", &id) != 1) {
                    printf("Error: Invalid numeric ID format.\n");
                    break;
                }
                find_task(tasks, count, id);
                break;
            }
            case CMD_HELP:
                print_help();
                break;
            case CMD_EXIT:
                printf("Exiting program. Freeing all allocated memory...\n");
                free(tasks);
                return 0;
            case CMD_UNKNOWN:
            default:
                printf("Error: Unrecognized command '%s'. Type 'HELP' for valid commands.\n", cmd_str);
                break;
        }
    }

    free(tasks);
    return 0;
}
