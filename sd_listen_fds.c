#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "systemd/sd-daemon.h"
#include "sd_listen_fds_internal.h"

int sd_listen_fds(int unset) {
	const char *value;
	long pid;
	unsigned long fd, fds;
	char *end;
	int r;

	if (!(value = getenv(LISTEN_PID))) {
		r = 0; goto bail;
	}
	if (!*value) {
		r = -EINVAL; goto bail;
	}
	errno = 0;
	pid = strtol(value, &end, 0);
	if (*end || ((pid == LONG_MIN || pid == LONG_MAX) && errno == ERANGE)) {
		r = -errno; goto bail;
	}
	if (pid != getpid()) {
		r = 0; goto bail;
	}

	value = getenv(LISTEN_FDS);
	if (!value) {
		r = 0; goto bail;
	}
	if (!*value) {
		r = -EINVAL; goto bail;
	}
	errno = 0;
	fds = strtoul(value, &end, 0);
	if (*end || (fds == ULONG_MAX && errno == ERANGE)) {
		r = -errno; goto bail;
	}
	if (fds > INT_MAX - SD_LISTEN_FDS_START) {
		r = -ERANGE; goto bail;
	}

	for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + fds; fd++) {
		int flags;
		if ((flags = fcntl(fd, F_GETFD)) == -1) {
			r = -errno; goto bail;
		}
		flags |= FD_CLOEXEC;
		if (fcntl(fd, F_SETFD) == -1) {
			r = -errno; goto bail;
		}
	}
	r = fds;

bail:
	if (unset) {
		unsetenv(LISTEN_PID);
		unsetenv(LISTEN_FDS);
		unsetenv(LISTEN_FDNAMES);
	}
	return r;
}
