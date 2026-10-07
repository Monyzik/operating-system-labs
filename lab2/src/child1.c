#include "io.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    char *line = NULL;
    size_t capacity = 0;
    ssize_t size = getline(&line, &capacity, stdin);

    if (size == -1) {
        if (feof(stdin)) {
            fprintf(stderr, "child1: входные данные отсутствуют\n");
        } else {
            perror("child1: getline");
        }
        free(line);
        return EXIT_FAILURE;
    }
    for (ssize_t i = 0; i < size; ++i) {
        line[i] = toupper(line[i]);
    }

    if (WriteAll(STDOUT_FILENO, line, size) == -1) {
        perror("child1: write");
        free(line);
        return EXIT_FAILURE;
    }


    free(line);
    return EXIT_SUCCESS;
}
