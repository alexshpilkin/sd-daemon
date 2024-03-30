#include <unistd.h>

#include "systemd/sd-daemon.h"

int sd_pid_notify(pid_t pid, int unset, const char *str) {
	return sd_pid_notify_with_fds(pid, unset, str, NULL, 0);
}
