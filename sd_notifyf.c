#include <errno.h>
#include <stdlib.h>

#include "systemd/sd-daemon.h"
#include "sd_notifyf_internal.h"

int sd_notifyf(int unset, const char *fmt, ...) {
	char *buf;
	int r;
	VA_SPRINTF(&buf, fmt);
	if (!buf)
		return -errno;
	r = sd_pid_notify_with_fds(0, unset, buf, NULL, 0);
	free(buf);
	return r;
}
