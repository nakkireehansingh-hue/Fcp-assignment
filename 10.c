
/*
 * Program: Extended Generic Container using Enum, Union and Structure.
 *
 * Features:
 * 1. TYPE_EMPTY and safe initialization.
 * 2. Setters for int, float, char and string.
 * 3. Safe dynamic memory allocation.
 * 4. Type-safe getters.
 * 5. Safe memory cleanup.
 * 6. Container printing and type names.
 * 7. Array of seven containers.
 * 8. Deep copying.
 * 9. STRING -> INT -> STRING -> FLOAT transitions.
 * 10. Pointer validation and complete cleanup.
 *
 * Sample output:
 * [0] INT: 10
 * [1] FLOAT: 3.140000
 * [2] CHAR: A
 * [3] STRING: Hello
 * [4] INT: 0
 * [5] STRING: Data
 * [6] STRING: Data
 *
 * Edge cases tested:
 * Integer zero, wrong-type getter, empty container,
 * empty string, allocation failure, switching from string,
 * repeated clear, deep copy, self-copy and same-pointer
 * string replacement.
 *
 * Compile:
 * gcc -std=c11 -Wall -Wextra -Wpedantic 10.c -o 10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Enumeration of supported data types */

typedef enum {
    TYPE_EMPTY = 0,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} ContainerType;

/* Union stores one value at a time */

typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} ContainerData;

/* Structure combines data and its type */

typedef struct {
    ContainerData data;
    ContainerType type;
} Container;

/*
 * Ownership rule:
 * Never shallow-copy a Container containing an owned string.
 * Use container_copy() for copying.
 */

/* Allocation failure simulation for testing */

static int fail_next_string_allocation = 0;

/* Checked memory allocation */

static void *container_allocate(size_t size)
{
    if (fail_next_string_allocation != 0) {
        fail_next_string_allocation = 0;
        return NULL;
    }

    return malloc(size);
}

/* Create an independent copy of a string */

static char *duplicate_string(const char *source)
{
    size_t length;
    char *copy;

    if (source == NULL) {
        return NULL;
    }

    length = strlen(source);

    if (length == (size_t)-1) {
        return NULL;
    }

    copy = container_allocate(length + 1U);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, source, length + 1U);

    return copy;
}

/* Initialize container */

int container_init(Container *container)
{
    if (container == NULL) {
        return 0;
    }

    container->data.string_value = NULL;
    container->type = TYPE_EMPTY;

    return 1;
}

/* Clear container and release owned memory */

void container_clear(Container *container)
{
    if (container == NULL) {
        return;
    }

    if (container->type == TYPE_STRING) {
        free(container->data.string_value);
    }

    container->data.string_value = NULL;
    container->type = TYPE_EMPTY;
}

/* Set integer value */

int container_set_int(Container *container, int value)
{
    if (container == NULL) {
        return 0;
    }

    container_clear(container);

    container->data.int_value = value;
    container->type = TYPE_INT;

    return 1;
}

/* Set float value */

int container_set_float(Container *container, float value)
{
    if (container == NULL) {
        return 0;
    }

    container_clear(container);

    container->data.float_value = value;
    container->type = TYPE_FLOAT;

    return 1;
}

/* Set character value */

int container_set_char(Container *container, char value)
{
    if (container == NULL) {
        return 0;
    }

    container_clear(container);

    container->data.char_value = value;
    container->type = TYPE_CHAR;

    return 1;
}

/* Set string value using independent heap allocation */

int container_set_string(Container *container, const char *value)
{
    char *new_string;

    if (container == NULL || value == NULL) {
        return 0;
    }

    /*
     * Allocate before clearing the old value.
     * This preserves the previous value if allocation fails.
     * It also supports passing the current internal string pointer.
     */

    new_string = duplicate_string(value);

    if (new_string == NULL) {
        return 0;
    }

    container_clear(container);

    container->data.string_value = new_string;
    container->type = TYPE_STRING;

    return 1;
}

/* Type-safe integer getter */

int container_get_int(const Container *container, int *out_value)
{
    if (container == NULL || out_value == NULL ||
        container->type != TYPE_INT) {
        return 0;
    }

    *out_value = container->data.int_value;

    return 1;
}

/* Type-safe float getter */

int container_get_float(const Container *container, float *out_value)
{
    if (container == NULL || out_value == NULL ||
        container->type != TYPE_FLOAT) {
        return 0;
    }

    *out_value = container->data.float_value;

    return 1;
}

/* Type-safe character getter */

int container_get_char(const Container *container, char *out_value)
{
    if (container == NULL || out_value == NULL ||
        container->type != TYPE_CHAR) {
        return 0;
    }

    *out_value = container->data.char_value;

    return 1;
}

/* Type-safe string getter */

int container_get_string(
    const Container *container,
    const char **out_value)
{
    if (container == NULL || out_value == NULL ||
        container->type != TYPE_STRING ||
        container->data.string_value == NULL) {
        return 0;
    }

    *out_value = container->data.string_value;

    return 1;
}

/* Return readable type name */

const char *container_type_name(const Container *container)
{
    if (container == NULL) {
        return "INVALID";
    }

    switch (container->type) {
        case TYPE_EMPTY:
            return "EMPTY";

        case TYPE_INT:
            return "INT";

        case TYPE_FLOAT:
            return "FLOAT";

        case TYPE_CHAR:
            return "CHAR";

        case TYPE_STRING:
            return "STRING";

        default:
            return "INVALID";
    }
}

/* Display value according to its active type */

int container_print(const Container *container)
{
    if (container == NULL) {
        return 0;
    }

    switch (container->type) {

        case TYPE_EMPTY:
            if (printf("EMPTY") < 0) {
                return 0;
            }
            break;

        case TYPE_INT:
            if (printf("%d", container->data.int_value) < 0) {
                return 0;
            }
            break;

        case TYPE_FLOAT:
            if (printf("%f",
                       (double)container->data.float_value) < 0) {
                return 0;
            }
            break;

        case TYPE_CHAR:
            if (printf("%c", container->data.char_value) < 0) {
                return 0;
            }
            break;

        case TYPE_STRING:
            if (container->data.string_value == NULL) {
                return 0;
            }

            if (printf("%s",
                       container->data.string_value) < 0) {
                return 0;
            }
            break;

        default:
            return 0;
    }

    return 1;
}

/* Deep copy of one container into another */

int container_copy(
    Container *destination,
    const Container *source)
{
    char *string_copy = NULL;

    if (destination == NULL || source == NULL) {
        return 0;
    }

    /* Self-copy is a successful no-operation */

    if (destination == source) {
        return 1;
    }

    /*
     * Allocate a separate string before clearing destination.
     * Allocation failure leaves destination unchanged.
     */

    if (source->type == TYPE_STRING) {

        if (source->data.string_value == NULL) {
            return 0;
        }

        string_copy = duplicate_string(
            source->data.string_value);

        if (string_copy == NULL) {
            return 0;
        }

    } else if (source->type != TYPE_EMPTY &&
               source->type != TYPE_INT &&
               source->type != TYPE_FLOAT &&
               source->type != TYPE_CHAR) {

        return 0;
    }

    container_clear(destination);

    switch (source->type) {

        case TYPE_EMPTY:
            destination->type = TYPE_EMPTY;
            destination->data.string_value = NULL;
            break;

        case TYPE_INT:
            destination->data.int_value =
                source->data.int_value;

            destination->type = TYPE_INT;
            break;

        case TYPE_FLOAT:
            destination->data.float_value =
                source->data.float_value;

            destination->type = TYPE_FLOAT;
            break;

        case TYPE_CHAR:
            destination->data.char_value =
                source->data.char_value;

            destination->type = TYPE_CHAR;
            break;

        case TYPE_STRING:
            destination->data.string_value = string_copy;
            destination->type = TYPE_STRING;
            break;

        default:
            return 0;
    }

    return 1;
}

/* Print index, type and value */

static void print_indexed_container(
    size_t index,
    const Container *container)
{
    printf("[%zu] %s: ",
           index,
           container_type_name(container));

    if (!container_print(container)) {
        printf("<invalid container>");
    }

    putchar('\n');
}

/* Test all required edge cases */

static void run_edge_case_tests(void)
{
    Container test;
    Container copy;

    int int_result = -1;
    const char *string_result = NULL;

    assert(container_init(&test));
    assert(container_init(&copy));

    /* Empty container and repeated clear */

    assert(test.type == TYPE_EMPTY);

    container_clear(&test);
    container_clear(&test);

    assert(test.type == TYPE_EMPTY);

    /* Valid integer zero */

    assert(container_set_int(&test, 0));

    assert(container_get_int(&test, &int_result));
    assert(int_result == 0);

    /* Wrong-type getter */

    assert(!container_get_float(&test, &(float){0.0f}));

    /* Empty string */

    assert(container_set_string(&test, ""));

    assert(container_get_string(&test, &string_result));
    assert(strcmp(string_result, "") == 0);

    /* Allocation failure must preserve old value */

    assert(container_set_int(&test, 27));

    fail_next_string_allocation = 1;

    assert(!container_set_string(
        &test, "allocation must fail"));

    assert(container_get_int(&test, &int_result));
    assert(int_result == 27);

    /* Switching from string to integer */

    assert(container_set_string(&test, "temporary"));
    assert(container_set_int(&test, 42));

    assert(container_get_int(&test, &int_result));
    assert(int_result == 42);

    /* Deep copy of a string */

    assert(container_set_string(&test, "original"));

    assert(container_copy(&copy, &test));

    assert(container_set_string(&test, "changed"));

    assert(container_get_string(&copy, &string_result));
    assert(strcmp(string_result, "original") == 0);

    /* Self-copy */

    assert(container_copy(&copy, &copy));

    assert(container_get_string(&copy, &string_result));
    assert(strcmp(string_result, "original") == 0);

    /* Replace string using its own internal pointer */

    assert(container_get_string(&test, &string_result));

    assert(container_set_string(&test, string_result));

    assert(container_get_string(&test, &string_result));
    assert(strcmp(string_result, "changed") == 0);

    /* Repeated cleanup */

    container_clear(&test);
    container_clear(&test);

    container_clear(&copy);
    container_clear(&copy);
}

/* Main function */

int main(void)
{
    Container containers[7];

    size_t index;

    /* Initialize all seven containers */

    for (index = 0U; index < 7U; index++) {

        if (!container_init(&containers[index])) {

            fprintf(stderr,
                    "Failed to initialize container %zu\n",
                    index);

            return EXIT_FAILURE;
        }
    }

    /* Populate containers with different data types */

    if (!container_set_int(&containers[0], 10) ||
        !container_set_float(&containers[1], 3.14f) ||
        !container_set_char(&containers[2], 'A') ||
        !container_set_string(&containers[3], "Hello") ||
        !container_set_int(&containers[4], 0) ||
        !container_set_string(&containers[5], "Data")) {

        fprintf(stderr, "Failed to populate containers\n");

        for (index = 0U; index < 7U; index++) {
            container_clear(&containers[index]);
        }

        return EXIT_FAILURE;
    }

    /* Deep copy the last string container */

    if (!container_copy(&containers[6], &containers[5])) {

        fprintf(stderr, "Failed to copy container\n");

        for (index = 0U; index < 7U; index++) {
            container_clear(&containers[index]);
        }

        return EXIT_FAILURE;
    }

    /* Display all containers */

    printf("\n--- Container Array ---\n");

    for (index = 0U; index < 7U; index++) {
        print_indexed_container(index, &containers[index]);
    }

    /* Change original string and verify independent copy */

    if (!container_set_string(&containers[5], "Changed")) {

        fprintf(stderr, "String replacement failed\n");

        for (index = 0U; index < 7U; index++) {
            container_clear(&containers[index]);
        }

        return EXIT_FAILURE;
    }

    printf("\nOriginal after replacement: ");

    container_print(&containers[5]);

    putchar('\n');

    printf("Deep copy after source change: ");

    container_print(&containers[6]);

    putchar('\n');

    /* Demonstrate STRING -> INT -> STRING -> FLOAT */

    if (!container_set_string(&containers[0], "first") ||
        !container_set_int(&containers[0], 99) ||
        !container_set_string(&containers[0], "second") ||
        !container_set_float(&containers[0], 2.5f)) {

        fprintf(stderr, "Type transition failed\n");

        for (index = 0U; index < 7U; index++) {
            container_clear(&containers[index]);
        }

        return EXIT_FAILURE;
    }

    printf("\nSTRING -> INT -> STRING -> FLOAT completed.\n");

    /* Run edge-case tests */

    run_edge_case_tests();

    /* Clean up the entire array */

    for (index = 0U; index < 7U; index++) {
        container_clear(&containers[index]);
    }

    printf("\nAll edge-case checks passed.\n");

    return EXIT_SUCCESS;
}
