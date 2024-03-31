#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "systemd/sd-daemon.h"

int sd_is_socket(int fd, int family, int type, int listening) {
	int opt;
	socklen_t len;

	if (family != AF_UNSPEC) {
		struct sockaddr sa;
		len = sizeof sa;
		if (getsockname(fd, &sa, &len) < 0)
			return errno != ENOTSOCK ? -errno : 0;
		assert(len >= offsetof(struct sockaddr, sa_family) + sizeof sa.sa_family);
		if (sa.sa_family != family)
			return 0;
	} else {
		struct stat st;
		if (fstat(fd, &st) < 0)
			return -errno;
		if (!S_ISSOCK(st.st_mode))
			return 0;
	}
	if (type) {
		len = sizeof opt;
		if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &opt, &len) < 0)
			return -errno;
		assert(len >= sizeof opt);
		if (opt != type)
			return 0;
	}
	if (listening >= 0) {
#ifdef SO_ACCEPTCONN
		len = sizeof opt;
		if (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &opt, &len) < 0)
			return -errno;
		assert(len >= sizeof opt);
		if (!!opt != !!listening)
			return 0;
#else
		return -EINVAL;
#endif
	}
	return 1;
}
