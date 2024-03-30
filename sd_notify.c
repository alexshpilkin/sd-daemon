#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "systemd/sd-daemon.h"

int sd_notify(int unset, const char *str) {
	int fd, r;
	if (!(fd = sd_notify_socket(unset)))
		return fd;
	r = send(fd, str, str ? strlen(str) : 0, MSG_NOSIGNAL) < 0 ? -errno : 0;
	close(fd);
	return r;
}
