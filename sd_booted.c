#include <errno.h>
#include <sys/stat.h>

#include "systemd/sd-daemon.h"

int sd_booted(void) {
	struct stat st;
	if (stat("/run/systemd/system", &st) >= 0)
		return !!S_ISDIR(st.st_mode);
	if (errno != ENOENT && errno != ENOTDIR)
		return -errno;
	return 0;
}
