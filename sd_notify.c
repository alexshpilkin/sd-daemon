#include <stddef.h>

#include "systemd/sd-daemon.h"

int sd_notify(int unset, const char *str) {
	return sd_pid_notify_with_fds(0, unset, str, NULL, 0);
}
