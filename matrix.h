/**
 * CITS3402: High Performance Computing Project 1
 * Optimising performance with sparse matrix operations
 *
 * @author Bruce How (22242664)
 * @date 03/09/2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <omp.h>
#include <time.h>

#define MEMSIZ 8

// Classifying matrix types
enum mat_type {INT, FLOAT};

// Processing functions
char *read_line(FILE *fp);

// Format functions
extern struct COO coo_format(int rows, int cols, char *values);

// Memory allocations
void *allocate(size_t size);
void *reallocate(void *ptr, size_t size);

// Coordinate Format (COO)
struct ELEMENT {
    int x;
    int y;
    int value;
};

struct ELEMENT_F {
    int x;
    int y;
    float value;
};

struct COO {
    int length;
    struct ELEMENT *elements;
};

struct COO_F {
    int length;
    struct ELEMENT_F *elements;
};
