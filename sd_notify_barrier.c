#include <stdint.h>

#include "systemd/sd-daemon.h"

#ifdef UINT64_MAX
int sd_notify_barrier(int unset, uint64_t usec) {
	return sd_pid_notify_barrier(0, unset, usec);
}
#endif
