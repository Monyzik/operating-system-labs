#include "io.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int WriteAll(int fd, const char *data, size_t size) {
    while (size > 0) {
        ssize_t count = write(fd, data, size);

        if (count == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        if (count == 0) {
            errno = EIO;
            return -1;
        }

        data += count;
        size -= (size_t) count;
    }
    return 0;
}

int CloseFd(int *fd) {
    if (*fd == -1) {
        return 0;
    }

    int descriptor = *fd;
    *fd = -1;

    if (close(descriptor) == -1) {
        perror("close");
        return -1;
    }
    return 0;
}
