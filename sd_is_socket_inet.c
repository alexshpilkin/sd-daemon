#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <sys/socket.h>

#if defined AF_INET || defined AF_INET6
#include <netinet/in.h>
#endif

#include "systemd/sd-daemon.h"

int sd_is_socket_inet(int fd, int family, int type, int listening,
                      uint16_t port)
{
	union {
		struct sockaddr sa;
#ifdef AF_INET
		struct sockaddr_in in;
#endif
#ifdef AF_INET6
		struct sockaddr_in6 in6;
#endif
	} u;
	socklen_t len = sizeof u;
	int r;

	if ((r = sd_is_socket(fd, AF_UNSPEC, type, listening)) <= 0)
		return r;
	if (getsockname(fd, &u.sa, &len) < 0)
		return errno != EOPNOTSUPP ? -errno : 0;
	assert(len >= offsetof(struct sockaddr, sa_family) + sizeof u.sa.sa_family);
	if (family != AF_UNSPEC && family != u.sa.sa_family)
		return 0;
	switch (u.sa.sa_family) {
#ifdef AF_INET
	case AF_INET:
		assert(len >= offsetof(struct sockaddr_in, sin_port) + sizeof u.in.sin_port);
		return !port || u.in.sin_port == port;
#endif
#ifdef AF_INET6
	case AF_INET6:
		assert(len >= offsetof(struct sockaddr_in6, sin6_port) + sizeof u.in6.sin6_port);
		return !port || u.in6.sin6_port == port;
#endif
	default: return 0;
	}
}
