#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    char *line = NULL;
    size_t capacity = 0;
    int result = EXIT_FAILURE;
    ssize_t size = getline(&line, &capacity, stdin);

    if (size == -1) {
        if (feof(stdin)) {
            fprintf(stderr, "child1: входные данные отсутствуют\n");
        } else {
            perror("child1: getline");
        }
    } else {
        for (ssize_t i = 0; i < size; ++i) {
            line[i] = (char) toupper((unsigned char) line[i]);
        }

        if (WriteAll(STDOUT_FILENO, line, (size_t) size) == -1) {
            perror("child1: write");
        } else {
            result = EXIT_SUCCESS;
        }
    }

    free(line);
    return result;
}
