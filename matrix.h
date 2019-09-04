/**
 * CITS3402: High Performance Computing Project 1
 * Optimising performance with sparse matrices operations
 *
 * @author Bruce How (22242664)
 * @date 03/09/2019
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <opm.h>
#include <time.h>

// Global variables used as sparse matrix representations

// Coordinate Format (COO)
typedef struct {
    int coordinates[][];
} COO;