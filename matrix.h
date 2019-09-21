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
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>
#include <omp.h>
#include <sys/time.h>

#define MEMSIZ 8

// Output const
#define OUTPUT_NAME_SIZE 31
#define SID 22242664

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
 * @param scalar The int scalar value to multiply by
 */
extern void scalar_multiply(struct COO matrix, float scalar);

/**
 * Calculates the trace value of a given int matrix
 * 
 * @param matrix The matrix to calculate the trace value for
 * @return int The trace value
 */
extern int trace(struct CSR matrix);

/**
 * Caclulates the trace value of a given float matrix
 * 
 * @param matrix The mateix to calculate the trace value for
 * @return float The trace value
 */
extern float trace_f(struct CSR matrix);

/**
 * Performs matrix addition on two given integer matrices
 * 
 * @param matrix1 The first matrix
 * @param matrix2 The second matrix
 * @return struct CSR The resulting added matrix
 */
extern struct CSR matrix_addition(struct CSR matrix1, struct CSR matrix2);

/**
 * Performs matrix addition on two given float matrices
 * 
 * @param matrix1 The first matrix
 * @param matrix2 The second matrix
 * @return struct CSR The resulting added matrix
 */
extern struct CSR matrix_addition_f(struct CSR matrix, struct CSR matrix2);

/**
 * Transposes a given matrix
 * 
 * @param matrix The matrix to transpose
 * @return struct CSR The transposed matrix
 */
extern struct CSR transpose(struct CSC matrix);

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

/**
 * Get the output name for the output file given the current
 * date and time 
 *
 * @param tm The date time struct
 * @param routine The routine type
 * @return char* The name of the output file
 */
extern char *get_output_name(struct tm tm, char *routine);

/**
 * Writes the time taken to process and load data and the time taken
 * to execute the specified algebraic routine
 * 
 * @param fp  The file pointer to output to
 * @param load_time The process and load time in seconds
 * @param routine_time The algebraic routine execution time in seconds
 */
extern void write_times(FILE *fp, float load_time, float routine_time);

/**
 * Writes the COO structure data to the given file pointer.
 * The function assumes that the x,y values for each element
 * is sorted to take advantage of quicker printing algorithms
 * 
 * @param fp The file pointer to output to
 * @param matrix The matrix containing the data to write from
 */
extern void write_coo_data(FILE *fp, struct COO matrix);

/**
 * Writes the CSR structure data to the given file pointer.
 * 
 * @param fp The file pointer to output to
 * @param matrix The matrix containing the data to write from
 */
extern void write_csr_data(FILE *fp, struct CSR matrix);

/**
 * Writes the header details to the given file pointer
 * 
 * @param fp The file pointer to output to
 * @param filename The filename of the first matrix
 * @param filename2 The filename of the second matrix if any
 * @param rows The number of rows in the matrix
 * @param cols The number of columns in the matrix
 * @param routine The algebraic routine type
 * @param type The variable type of the matrix
 */
extern void write_details(FILE *fp, char* filename, char* filename2, int rows, int cols, enum ROUTINE_TYPE routine, enum VAR_TYPE type);

extern float get_time(struct timeval start, struct timeval end);

// COO representation
struct COO {
    enum VAR_TYPE type;
    int rows;
    int cols;
    int count; // Number of cordinate entries
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
    int cols;
    int count; // Number of NNZ values
    union {
        int *i;
        float *f;
    } nnz; // List of non-zero values
    int *ia; // Total number of elements up until specific row
    int *ja; // List of column index for each NNZ value
};

// CSC representation
struct CSC {
    enum VAR_TYPE type;
    int rows;
    int cols; // Used for IA length + 1
    int count; // Number of NNZ values
    union {
        int *i;
        float *f;
    } nnz; // List of non-zero values
    int *ia; // Total number of elements up until specific col
    int *ja; // List of row index for each NNZ value
};

// Routine representation
struct ROUTINE {
    enum ROUTINE_TYPE type;
    float scalar; // If SM is used
};