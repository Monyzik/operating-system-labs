#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    char *line = NULL;
    size_t capacity = 0;
    ssize_t size = getline(&line, &capacity, stdin);

    if (size == -1) {
        if (feof(stdin)) {
            fprintf(stderr, "child2: входные данные отсутствуют\n");
        } else {
            perror("child2: getline");
        }
        free(line);
        return EXIT_FAILURE;
    }
    size_t length = 0;

    for (ssize_t i = 0; i < size; ++i) {
        if (line[i] == ' ' && length > 0 && line[length - 1] == ' ') {
            continue;
        }
        line[length++] = line[i];
    }

    line[length] = '\0';

    if (WriteAll(STDOUT_FILENO, line, length) == -1) {
        perror("child2: write");
        free(line);
        return EXIT_FAILURE;
    }


    free(line);
    return EXIT_SUCCESS;
}
