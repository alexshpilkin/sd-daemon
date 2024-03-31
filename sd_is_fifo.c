#include <errno.h>
#include <sys/stat.h>

#include "systemd/sd-daemon.h"

int sd_is_fifo(int fd, const char *path) {
	struct stat st, pathst;
	if (fstat(fd, &st) < 0 || (path && stat(path, &pathst) < 0))
		return -errno;
	if (!S_ISFIFO(st.st_mode))
		return 0;
	if (path)
		return st.st_dev == pathst.st_dev && st.st_ino == pathst.st_ino;
	return 1;
}
