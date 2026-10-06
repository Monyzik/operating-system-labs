#ifndef LAB2_IO_H
#define LAB2_IO_H

#include <stddef.h>

int WriteAll(int fd, const char *data, size_t size);
int CloseFd(int *fd);

#endif
