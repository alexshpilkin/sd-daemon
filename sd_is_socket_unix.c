#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "systemd/sd-daemon.h"

int sd_is_socket_unix(int fd, int type, int listening,
                      const char *path, size_t length)
{
	struct sockaddr_un un;
	socklen_t len = sizeof un;
	int r;

	if ((r = sd_is_socket(fd, AF_UNSPEC, type, listening)) <= 0)
		return r;
	if (getsockname(fd, (struct sockaddr *)&un, &len) < 0)
		return errno != EOPNOTSUPP ? -errno : 0;
	assert(len >= offsetof(struct sockaddr_un, sun_family) + sizeof un.sun_family);
	if (un.sun_family != AF_UNIX)
		return 0;
	if (path) {
		assert(len >= offsetof(struct sockaddr_un, sun_path));
		assert(len <= offsetof(struct sockaddr_un, sun_path) + sizeof un.sun_path);
		if (!length)
			length = strlen(path);
		if (length != len - offsetof(struct sockaddr_un, sun_path))
			return 0;
		if (memcmp(un.sun_path, path, length))
			return 0;
	}
	return 1;
}
