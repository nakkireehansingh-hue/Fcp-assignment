#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#define EPSILON 1e-5f
#define MAX_CAPACITY_LIMIT 1000

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

// Function prototypes
void compute_metrics(Rectangle *r);
int find_index_by_id(Rectangle *rectangles, int count, int id);
void add_rectangle(Rectangle **rectangles, int *count, int *capacity, int id, float width, float height);
void display_rectangles(Rectangle *rectangles, int count);
void search_rectangle(Rectangle *rectangles, int count, int id);
void update_rectangle(Rectangle *rectangles, int count, int id, float new_w, float new_h);
void delete_rectangle(Rectangle *rectangles, int *count, int id);
void display_statistics(Rectangle *rectangles, int count);
int compare_rectangles(const void *a, const void *b);
void display_sorted(Rectangle *rectangles, int count);
void filter_by_area(Rectangle *rectangles, int count, float threshold);
void free_rectangle_array(Rectangle *rectangles);

// Helper to compute derived geometric properties
void compute_metrics(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2.0f * (r->width + r->height);
}

// Search helper returning index or -1
int find_index_by_id(Rectangle *rectangles, int count, int id) {
    for (int i = 0; i < count; i++) {
        if (rectangles[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Add a rectangle with dynamic capacity resizing (realloc)
void add_rectangle(Rectangle **rectangles, int *count, int *capacity, int id, float width, float height) {
    // Validate unique ID
    if (find_index_by_id(*rectangles, *count, id) != -1) {
        printf("Error: Rectangle with ID %d already exists.\n", id);
        return;
    }

    // Validate finite positive dimensions
    if (width <= 0.0f || height <= 0.0f || isnan(width) || isnan(height) || isinf(width) || isinf(height)) {
        printf("Error: Dimensions must be finite positive numbers.\n");
        return;
    }

    // Check if capacity needs expansion
    if (*count >= *capacity) {
        int new_capacity = *capacity * 2;
        if (new_capacity > MAX_CAPACITY_LIMIT) {
            new_capacity = MAX_CAPACITY_LIMIT;
        }
        if (*count >= new_capacity) {
            printf("Error: Maximum capacity limit (%d) reached.\n", MAX_CAPACITY_LIMIT);
            return;
        }

        // Safe realloc keeping original pointer on failure
        Rectangle *temp = (Rectangle *)realloc(*rectangles, new_capacity * sizeof(Rectangle));
        if (temp == NULL) {
            printf("Error: Memory reallocation failed. Could not expand capacity.\n");
            return;
        }
        *rectangles = temp;
        *capacity = new_capacity;
    }

    // Insert new record
    (*rectangles)[*count].id = id;
    (*rectangles)[*count].width = width;
    (*rectangles)[*count].height = height;
    compute_metrics(&(*rectangles)[*count]);
    (*count)++;

    printf("Success: Rectangle ID %d added successfully.\n", id);
}

// Display all rectangles with square classification
void display_rectangles(Rectangle *rectangles, int count) {
    if (count == 0) {
        printf("\n[Collection is empty]\n");
        return;
    }

    printf("\n=========================================================\n");
    printf("                  RECTANGLE INVENTORY                    \n");
    printf("=========================================================\n");
    for (int i = 0; i < count; i++) {
        bool is_square = fabsf(rectangles[i].width - rectangles[i].height) < EPSILON;
        printf("ID: %-4d | Width: %-6.2f | Height: %-6.2f | Area: %-7.2f | Perimeter: %-7.2f | [%s]\n",
               rectangles[i].id,
               rectangles[i].width,
               rectangles[i].height,
               rectangles[i].area,
               rectangles[i].perimeter,
               is_square ? "Square" : "Non-Square");
    }
    printf("=========================================================\n");
}

// Search by ID
void search_rectangle(Rectangle *rectangles, int count, int id) {
    int idx = find_index_by_id(rectangles, count, id);
    if (idx == -1) {
        printf("Search Result: Rectangle with ID %d not found.\n", id);
        return;
    }

    bool is_square = fabsf(rectangles[idx].width - rectangles[idx].height) < EPSILON;
    printf("\n--- Found Rectangle ID %d ---\n", id);
    printf("Width: %.2f, Height: %.2f, Area: %.2f, Perimeter: %.2f (%s)\n",
           rectangles[idx].width, rectangles[idx].height,
           rectangles[idx].area, rectangles[idx].perimeter,
           is_square ? "Square" : "Non-Square");
}

// Update dimensions by ID
void update_rectangle(Rectangle *rectangles, int count, int id, float new_w, float new_h) {
    int idx = find_index_by_id(rectangles, count, id);
    if (idx == -1) {
        printf("Error: Rectangle with ID %d not found for update.\n", id);
        return;
    }

    if (new_w <= 0.0f || new_h <= 0.0f || isnan(new_w) || isnan(new_h) || isinf(new_w) || isinf(new_h)) {
        printf("Error: New dimensions must be finite positive numbers.\n");
        return;
    }

    rectangles[idx].width = new_w;
    rectangles[idx].height = new_h;
    compute_metrics(&rectangles[idx]);

    printf("Success: Rectangle ID %d updated successfully.\n", id);
}

// Delete by ID (preserving other records by shifting)
void delete_rectangle(Rectangle *rectangles, int *count, int id) {
    int idx = find_index_by_id(rectangles, *count, id);
    if (idx == -1) {
        printf("Error: Rectangle with ID %d not found for deletion.\n", id);
        return;
    }

    for (int i = idx; i < *count - 1; i++) {
        rectangles[i] = rectangles[i + 1];
    }
    (*count)--;
    printf("Success: Rectangle ID %d deleted successfully.\n", id);
}

// Display statistical summary
void display_statistics(Rectangle *rectangles, int count) {
    if (count == 0) {
        printf("\nStatistics Summary: Collection is empty.\n");
        return;
    }

    int min_idx = 0;
    int max_idx = 0;
    float total_area = 0.0f;

    for (int i = 0; i < count; i++) {
        total_area += rectangles[i].area;
        if (rectangles[i].area < rectangles[min_idx].area) {
            min_idx = i;
        }
        if (rectangles[i].area > rectangles[max_idx].area) {
            max_idx = i;
        }
    }

    float avg_area = total_area / count;

    printf("\n================ GEOMETRY STATISTICS ================\n");
    printf("Total Rectangles : %d\n", count);
    printf("Total Area       : %.2f\n", total_area);
    printf("Average Area     : %.2f\n", avg_area);
    printf("Smallest Area    : ID %d (Area: %.2f)\n", rectangles[min_idx].id, rectangles[min_idx].area);
    printf("Largest Area     : ID %d (Area: %.2f)\n", rectangles[max_idx].id, rectangles[max_idx].area);
    printf("=====================================================\n");
}

// Comparator for qsort: Ascending order of area, tie-break by ID ascending
int compare_rectangles(const void *a, const void *b) {
    const Rectangle *r1 = (const Rectangle *)a;
    const Rectangle *r2 = (const Rectangle *)b;

    if (r1->area < r2->area) return -1;
    if (r1->area > r2->area) return 1;

    // Tie-breaker by ID
    if (r1->id < r2->id) return -1;
    if (r1->id > r2->id) return 1;
    return 0;
}

// Display sorted list (operates on a working copy to preserve original insertion order)
void display_sorted(Rectangle *rectangles, int count) {
    if (count == 0) {
        printf("\n[Collection is empty]\n");
        return;
    }

    // Allocate temporary copy for sorting
    Rectangle *copy = (Rectangle *)malloc(count * sizeof(Rectangle));
    if (copy == NULL) {
        printf("Memory allocation failed for sorting.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        copy[i] = rectangles[i];
    }

    qsort(copy, count, sizeof(Rectangle), compare_rectangles);

    printf("\n--- Rectangles Sorted by Area (Ascending) ---\n");
    for (int i = 0; i < count; i++) {
        bool is_square = fabsf(copy[i].width - copy[i].height) < EPSILON;
        printf("ID: %-4d | Area: %-7.2f | Width: %.2f | Height: %.2f | [%s]\n",
               copy[i].id, copy[i].area, copy[i].width, copy[i].height,
               is_square ? "Square" : "Non-Square");
    }

    free(copy);
}

// Filter and display rectangles exceeding a given area threshold
void filter_by_area(Rectangle *rectangles, int count, float threshold) {
    if (count == 0) {
        printf("\n[Collection is empty]\n");
        return;
    }

    bool found = false;
    printf("\n--- Rectangles with Area > %.2f ---\n", threshold);
    for (int i = 0; i < count; i++) {
        if (rectangles[i].area > threshold) {
            printf("ID: %-4d | Area: %-7.2f (Width: %.2f, Height: %.2f)\n",
                   rectangles[i].id, rectangles[i].area, rectangles[i].width, rectangles[i].height);
            found = true;
        }
    }
    if (!found) {
        printf("No rectangles found exceeding area %.2f.\n", threshold);
    }
}

// Deallocate memory
void free_rectangle_array(Rectangle *rectangles) {
    free(rectangles);
}

int main() {
    int initial_capacity = 2;
    int count = 0;

    // Initial dynamic allocation
    Rectangle *rectangles = (Rectangle *)malloc(initial_capacity * sizeof(Rectangle));
    if (rectangles == NULL) {
        printf("Initial memory allocation failed.\n");
        return 1;
    }

    int choice;
    do {
        printf("\n=== Rectangle Inventory & Geometry System ===\n");
        printf("1. Add Rectangle\n");
        printf("2. Display All Rectangles\n");
        printf("3. Search by ID\n");
        printf("4. Update Dimensions by ID\n");
        printf("5. Delete by ID\n");
        printf("6. Geometry Statistics (Total, Avg, Min, Max)\n");
        printf("7. Display Sorted by Area (Ascending)\n");
        printf("8. Filter by Area Threshold\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a valid integer choice.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }

        switch (choice) {
            case 1: {
                int id;
                float w, h;
                printf("Enter Rectangle ID (positive integer): ");
                scanf("%d", &id);
                printf("Enter Width: ");
                scanf("%f", &w);
                printf("Enter Height: ");
                scanf("%f", &h);
                add_rectangle(&rectangles, &count, &initial_capacity, id, w, h);
                break;
            }
            case 2:
                display_rectangles(rectangles, count);
                break;
            case 3: {
                int id;
                printf("Enter Rectangle ID to search: ");
                scanf("%d", &id);
                search_rectangle(rectangles, count, id);
                break;
            }
            case 4: {
                int id;
                float w, h;
                printf("Enter Rectangle ID to update: ");
                scanf("%d", &id);
                printf("Enter New Width: ");
                scanf("%f", &w);
                printf("Enter New Height: ");
                scanf("%f", &h);
                update_rectangle(rectangles, count, id, w, h);
                break;
            }
            case 5: {
                int id;
                printf("Enter Rectangle ID to delete: ");
                scanf("%d", &id);
                delete_rectangle(rectangles, &count, id);
                break;
            }
            case 6:
                display_statistics(rectangles, count);
                break;
            case 7:
                display_sorted(rectangles, count);
                break;
            case 8: {
                float threshold;
                printf("Enter area threshold: ");
                scanf("%f", &threshold);
                filter_by_area(rectangles, count, threshold);
                break;
            }
            case 9:
                printf("Exiting program. Freeing all allocated memory...\n");
                break;
            default:
                printf("Invalid choice. Please select between 1 and 9.\n");
        }
    } while (choice != 9);

    free_rectangle_array(rectangles);
    return 0;
}
