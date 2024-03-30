#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define VA_SPRINTF(BUF, FMT) do { \
	va_list ap; \
	int size; \
	\
	va_start(ap, FMT); \
	size = vsnprintf(NULL, 0, FMT, ap); \
	va_end(ap); \
	if (size >= 0 && (unsigned)size >= (size_t)-1) { \
		errno = ENOMEM; *(BUF) = NULL; \
	} else if (size < 0 || !(*(BUF) = malloc((size_t)size + 1))) { \
		*(BUF) = NULL; \
	} else { \
		va_start(ap, FMT); \
		vsnprintf(*(BUF), size, FMT, ap); \
		va_end(ap); \
	} \
} while (0)
