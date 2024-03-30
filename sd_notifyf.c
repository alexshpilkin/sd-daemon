#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "systemd/sd-daemon.h"

int sd_notifyf(int unset, const char *fmt, ...) {
	va_list ap;
	int size;
	char *buf;
	int r;

	va_start(ap, fmt);
	size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	if (size >= 0 && (unsigned)size >= (size_t)-1)
		return -ENOMEM;
	if (size < 0 || !(buf = malloc((size_t)size + 1)))
		return -errno;
	va_start(ap, fmt);
	vsnprintf(buf, size, fmt, ap);
	va_end(ap);

	r = sd_notify(unset, buf);

	free(buf);
	return r;
}
