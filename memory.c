#include "matrix.h"

void *allocate(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        perror(__func__);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *reallocate(void* ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (ptr == NULL) {
        perror(__func__);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *callocate(size_t nitems, size_t size) {
    void *ptr = calloc(nitems, size);
    if (ptr == NULL) {
        perror(__func__);
        exit(EXIT_FAILURE);
    }
    return ptr;
}