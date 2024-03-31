#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "systemd/sd-daemon.h"

#ifdef UINT64_MAX
int sd_pid_notify_barrier(pid_t pid, int unset, uint64_t usec) {
	int rw[2], r;
	struct pollfd pfd = { 0 };
	struct timespec timeout;

	if (pipe(rw) < 0)
		return -errno;
	if ((r = sd_pid_notify_with_fds(pid, unset, "BARRIER=1", &rw[1], 1)) <= 0) {
		close(rw[0]); close(rw[1]); return r;
	}
	close(rw[1]);

	pfd.fd = rw[0];
	timeout.tv_sec  = usec / 1000000;
	timeout.tv_nsec = usec % 1000000 * 1000;
	if ((r = ppoll(&pfd, 1, usec != UINT64_MAX ? &timeout : NULL, NULL)) <= 0)
		r = r ? -errno : -ETIMEDOUT;
	close(rw[0]);

	return r;
}
#endif
