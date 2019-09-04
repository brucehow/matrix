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


// Matrix type classifier
enum mat_type {INT, FLOAT};

/**
 * Read the current line of a given file pointer
 * and appropriately allocate sufficient memory
 * to contain the file content
 * 
 * @param fp The file pointer of the input file
 * @return char* The content of line respective of the file pointer
 */
extern char *read_line(FILE *fp);

/**
 * Represents a given matrix using the Coordinate Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param values The data string of the matrix
 * @return struct COO The matrix representation
 */
extern struct COO coo_format(int rows, int cols, char *values);

/**
 * Allocates memory of a given size using malloc
 * 
 * @param size Size of the memory allocation
 * @return void* A pointer to the newly allocated memory
 */
void *allocate(size_t size);

/**
 * Reallocates memory to a specific size for a given pointer
 * 
 * @param ptr The pointer to reallocate memory to
 * @param size Size of the memory reallocation
 * @return void* A pointer to the newly reallocated memory
 */
void *reallocate(void *ptr, size_t size);


// Structure for the COO maxtrix representation
struct COO {
    int length;
    struct ELEMENT {
        int x; // The x coordinate
        int y; // The y coordinate
        int value; // Integer matrix value
    } *elements;
};

struct COO_F {
    int length;
    struct ELEMENT_F {
        int x;
        int y;
        float value; // Floating point matrix value
    } *elements;
};

