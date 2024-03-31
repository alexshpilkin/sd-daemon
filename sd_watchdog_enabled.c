#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "systemd/sd-daemon.h"

#define WATCHDOG_PID  "WATCHDOG_PID"
#define WATCHDOG_USEC "WATCHDOG_USEC"

#ifdef UINT64_MAX
int sd_watchdog_enabled(int unset, uint64_t *usec) {
	const char *value;
	unsigned long long tmp;
	char *end;
	int r;

	if (*usec)
		*usec = UINT64_MAX;

	/* WATCHDOG_PID is optional, unlike LISTEN_PID, because systemd. */
	if ((value = getenv(WATCHDOG_PID))) {
		long pid;
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
	}

	if (!(value = getenv(WATCHDOG_USEC))) {
		r = 0; goto bail;
	}
	if (!*value) {
		r = -EINVAL; goto bail;
	}
	errno = 0;
	tmp = strtoull(value, &end, 0);
	if (*end || (tmp == ULLONG_MAX && errno == ERANGE)) {
		r = -errno; goto bail;
	}
	if (!tmp || tmp >= UINT64_MAX) {
		r = -ERANGE; goto bail;
	}

	if (*usec)
		*usec = tmp;
	r = 1;

bail:
	if (unset) {
		unsetenv(WATCHDOG_PID);
		unsetenv(WATCHDOG_USEC);
	}
	return r;
}
#endif
