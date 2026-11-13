#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float width;
    float height;
    float area;
} Rectangle;

// Function to calculate the area of a single rectangle
void calculate_area(Rectangle *r) {
    r->area = r->width * r->height;
}

// Function to allocate memory for an array of rectangles with a safety check
Rectangle* create_rectangle_array(int size) {
    Rectangle *rectangles = (Rectangle*) malloc(size * sizeof(Rectangle));
    if (rectangles == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    return rectangles;
}

// Function to set dimensions and compute area for a specific index
void add_rectangle(Rectangle *rectangles, int index, float width, float height) {
    rectangles[index].width = width;
    rectangles[index].height = height;
    calculate_area(&rectangles[index]);
}

// Function to display all rectangles
void display_rectangles(Rectangle *rectangles, int size) {
    printf("\n--- Rectangle Summary ---\n");
    for (int i = 0; i < size; i++) {
        printf("Rectangle %d: Width = %.2f, Height = %.2f, Area = %.2f\n", 
               i + 1, rectangles[i].width, rectangles[i].height, rectangles[i].area);
    }
}

// NEW FUNCTION: Find and print the rectangle with the largest area
void find_largest_rectangle(Rectangle *rectangles, int size) {
    if (size <= 0) return;

    int max_index = 0;
    for (int i = 1; i < size; i++) {
        if (rectangles[i].area > rectangles[max_index].area) {
            max_index = i;
        }
    }

    printf("\n--- Largest Rectangle Found ---\n");
    printf("Rectangle %d has the maximum area: Width = %.2f, Height = %.2f, Area = %.2f\n",
           max_index + 1, rectangles[max_index].width, rectangles[max_index].height, rectangles[max_index].area);
}

// Function to free allocated memory
void free_rectangle_array(Rectangle *rectangles) {
    free(rectangles);
}

int main() {
    int number_of_rectangles;

    printf("Enter the number of rectangles: ");
    if (scanf("%d", &number_of_rectangles) != 1 || number_of_rectangles <= 0) {
        printf("Invalid input for number of rectangles.\n");
        return 1;
    }

    // Allocate memory dynamically
    Rectangle *rectangles = create_rectangle_array(number_of_rectangles);

    // Get dimensions from user input for each rectangle
    for (int i = 0; i < number_of_rectangles; i++) {
        float w, h;
        printf("\nEnter dimensions for Rectangle %d:\n", i + 1);
        printf("  Width: ");
        scanf("%f", &w);
        printf("  Height: ");
        scanf("%f", &h);

        add_rectangle(rectangles, i, w, h);
    }

    // Display all entered rectangles
    display_rectangles(rectangles, number_of_rectangles);

    // Find and display the largest rectangle
    find_largest_rectangle(rectangles, number_of_rectangles);

    // Clean up memory
    free_rectangle_array(rectangles);

    return 0;
}
