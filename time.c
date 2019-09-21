#include "matrix.h"

float get_time(struct timeval start, struct timeval end) {
    return ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
}
