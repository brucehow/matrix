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

extern enum mat_type read_mat_type(FILE *fp);

extern int read_mat_dim(FILE *fp);

/**
 * Represents a given matrix using the Coordinate Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param type The matrix data variable type
 * @param data The data string of the matrix
 * @return struct COO The matrix representation
 */
extern struct COO coo_format(int rows, int cols, enum mat_type type, char *data);

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


// COO representation structs
struct COO {
    int length;
    enum mat_type type;
    struct ELEMENT {
        int x;
        int y;
        union { // Use of union to represent both int and float
            int i;
            float f;
        } value;
    } *elements;
};

// CSR representation structs
struct CSR {
    int rows;
    int count; // Number of nrz values
    int *nrz; // List of non-zero values
    int *ia; // Total number of elements up until specific row
    int *ja; // List of column index for each nrz value
};