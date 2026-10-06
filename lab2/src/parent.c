#include "io.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int ClosePipes(int pipes[3][2], int keep_read, int keep_write) {
    int failed = 0;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (pipes[i][j] != keep_read && pipes[i][j] != keep_write) {
                failed |= CloseFd(&pipes[i][j]) == -1;
            }
        }
    }
    return failed ? -1 : 0;
}

static pid_t StartChild(const char *program, int pipes[3][2], int index) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
    } else if (pid == 0) {
        if (dup2(pipes[index][0], STDIN_FILENO) == -1 ||
            dup2(pipes[index + 1][1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(EXIT_FAILURE);
        }

        if (ClosePipes(pipes, STDIN_FILENO, STDOUT_FILENO) == -1) {
            _exit(EXIT_FAILURE);
        }

        execl(program, program, (char *) NULL);
        perror(program);
        _exit(EXIT_FAILURE);
    }
    return pid;
}

static int WaitChildren(pid_t children[2]) {
    int failed = 0;

    for (int i = 0; i < 2; ++i) {
        if (children[i] == -1) {
            continue;
        }

        int status;
        pid_t result;

        do {
            result = waitpid(children[i], &status, 0);
        } while (result == -1 && errno == EINTR);

        if (result == -1) {
            perror("waitpid");
            failed = 1;
        } else if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {
            fprintf(stderr, "child%d завершился с ошибкой\n", i + 1);
            failed = 1;
        }
    }
    return failed ? -1 : 0;
}

static ssize_t Run(int pipes[3][2], pid_t children[2],
                   const char *input, size_t size, char **output) {
    for (int i = 0; i < 3; ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    const char *programs[] = {CHILD1_PATH, CHILD2_PATH};

    for (int i = 0; i < 2; ++i) {
        children[i] = StartChild(programs[i], pipes, i);
        if (children[i] == -1) {
            return -1;
        }
    }

    if (ClosePipes(pipes, pipes[2][0], pipes[0][1]) == -1) {
        return -1;
    }

    if (WriteAll(pipes[0][1], input, size) == -1) {
        perror("write input");
        return -1;
    }

    if (CloseFd(&pipes[0][1]) == -1) {
        return -1;
    }

    FILE *stream = fdopen(pipes[2][0], "r");
    if (stream == NULL) {
        perror("fdopen");
        return -1;
    }

    pipes[2][0] = -1;
    size_t capacity = 0;
    ssize_t result_size = getline(output, &capacity, stream);

    if (result_size == -1) {
        if (feof(stream)) {
            fprintf(stderr, "Результат не получен\n");
        } else {
            perror("getline result");
        }
    }

    if (fclose(stream) == EOF) {
        perror("fclose");
        return -1;
    }
    return result_size;
}

int main(void) {
    int pipes[3][2] = {{-1, -1}, {-1, -1}, {-1, -1}};
    pid_t children[2] = {-1, -1};
    char *input = NULL;
    char *output = NULL;
    size_t capacity = 0;
    ssize_t output_size = -1;
    int result = EXIT_FAILURE;
    const char prompt[] = "Введите строку: ";
    const char prefix[] = "Результат: ";

    if (WriteAll(STDOUT_FILENO, prompt, sizeof(prompt) - 1) == -1) {
        perror("write prompt");
    } else {
        ssize_t input_size = getline(&input, &capacity, stdin);

        if (input_size >= 0) {
            output_size = Run(pipes, children, input, (size_t) input_size, &output);
        } else if (feof(stdin)) {
            fprintf(stderr, "Ввод отсутствует\n");
        } else {
            perror("getline input");
        }
    }

    int close_result = ClosePipes(pipes, -1, -1);
    int wait_result = WaitChildren(children);

    if (output_size >= 0 && close_result == 0 && wait_result == 0) {
        if (WriteAll(STDOUT_FILENO, prefix, sizeof(prefix) - 1) == -1 ||
            WriteAll(STDOUT_FILENO, output, (size_t) output_size) == -1) {
            perror("write result");
        } else {
            result = EXIT_SUCCESS;
        }
    }

    free(input);
    free(output);
    return result;
}
