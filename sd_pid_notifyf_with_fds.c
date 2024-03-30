#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "sd-daemon.h"
#include "sd_notifyf_internal.h"

int sd_pid_notifyf_with_fds(pid_t pid, int unset,
                            const int *fds, size_t nfds,
                            const char *fmt, ...)
{
	char *buf;
	int r;
	VA_SPRINTF(&buf, fmt);
	if (!buf)
		return -errno;
	r = sd_pid_notify_with_fds(pid, unset, buf, fds, nfds);
	free(buf);
	return r;
}
