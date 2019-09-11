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
#include <errno.h>
#include <time.h>
#include <inttypes.h>
#include <omp.h>

#define MEMSIZ 8

// DEBUG MACROS
#define pint(x) printf("%s = %d\n", #x, x); fflush(stdout);
#define pstr(x) printf("%s = %s\n", #x, x); fflush(stdout);
#define p() printf("HERE\n");fflush(stdout);

// Matrix type classifier
enum VAR_TYPE {TYPE_INT, TYPE_FLOAT, INVALID};
enum ROUTINE_TYPE {SM, TR, AD, TS, MM, UNDEF};

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
 * Read the current line of a given file pointer
 * and determine the appropriate matrix data type
 * 
 * @param fp The file pointer of the input file
 * @return enum VAR_TYPE The data type of the matrix
 */
extern enum VAR_TYPE read_mat_type(FILE *fp);

/**
 * Read the current line of the given file pointer
 * and determine the matrix dimension
 *
 * @param fp The file pointer of the input file
 * @return int The matrix dimension, row/col value
 */
extern int read_mat_dim(FILE *fp);

/**
 * Determine the numeric data type of a given string
 * 
 * @param val The string to determine the data type for
 * @return enum VAR_TYPE The numeric data type
 */
extern enum VAR_TYPE numeric_type(char *val);

/**
 * Represents a given matrix using the Coordinate Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param type The matrix data variable type
 * @param data The data string of the matrix
 * @return struct COO The matrix representation
 */
extern struct COO coo_format(int rows, int cols, enum VAR_TYPE type, char *data);

/**
 * Represents a given matrix using the Compressed Sparse Row Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param type The matrix data variable type
 * @param data The data string of the matrix
 * @return struct CSR The matrix representation
 */
extern struct CSR csr_format(int rows, int cols, enum VAR_TYPE type, char *data);

/**
 * Represents a given matrix using the Compressed Sparse Column Format 
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @param type The matrix data variable type
 * @param data The data string of the matrix
 * @return struct CSC The matrix representation
 */
extern struct CSC csc_format(int rows, int cols, enum VAR_TYPE type, char *data);

/**
 * Performs scalar multiplication on the given matrix and the
 * given parameter (scalar value)
 *
 * @param matrix The matrix to perform the algebraic routine on
 * @param scalar The scalar value to multiply by
 */
extern void scalar_multiply(struct COO matrix, int scalar);

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
    enum VAR_TYPE type;
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
    enum VAR_TYPE type;
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
    enum VAR_TYPE type;
    int cols; // Used for IA length + 1
    int count; // Number of nnz values
    union {
        int *i;
        float *f;
    } nnz; // List of non-zero values
    int *ia; // Total number of elements up until specific col
    int *ja; // List of row index for each nnz value
};

// Routine representation
struct ROUTINE {
    enum ROUTINE_TYPE type;
    enum VAR_TYPE union_type;
    union {
        int i;
        float f;
    } param; // Used to store parameter required routines
};