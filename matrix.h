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
enum mat_type {INT_MAT, FLOAT_MAT};

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
 * Represents a given matrix using the Compressed Sparse Row Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param type The matrix data variable type
 * @param data The data string of the matrix
 * @return struct CSR The matrix representation
 */
extern struct CSR csr_format(int rows, int cols, enum mat_type type, char *data);

/**
 * Represents a given matrix using the Compressed Sparse Column Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param type The matrix data variable type
 * @param data The data string of the matrix
 * @return struct CSC The matrix representation
 */
extern struct CSC csc_format(int rows, int cols, enum mat_type type, char *data);

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


// COO representation
struct COO {
    enum mat_type type;
    int count;
    struct ELEMENT {
        int x;
        int y;
        union { // Use of union to represent both int and float
            int i;
            float f;
        } value;
    } *elements;
};

// CSR representation
struct CSR {
    enum mat_type type;
    int rows; // Used for IA length + 1
    int count; // Number of nnz values
    union {
        int *i;
        float *f;
    } nnz; // List of non-zero values
    int *ia; // Total number of elements up until specific row
    int *ja; // List of column index for each nnz value
};

// CSC representation
struct CSC {
    enum mat_type type;
    int cols; // Used for IA length + 1
    int count; // Number of nnz values
    union {
        int *i;
        float *f;
    } nnz; // List of non-zero values
    int *ia; // Total number of elements up until specific col
    int *ja; // List of row index for each nnz value
};