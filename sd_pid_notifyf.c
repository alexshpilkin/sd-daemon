#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "systemd/sd-daemon.h"
#include "sd_notifyf_internal.h"

int sd_pid_notifyf(pid_t pid, int unset, const char *fmt, ...) {
	char *buf;
	int r;
	VA_SPRINTF(&buf, fmt);
	if (!buf)
		return -errno;
	r = sd_pid_notify_with_fds(pid, unset, buf, NULL, 0);
	free(buf);
	return r;
}
