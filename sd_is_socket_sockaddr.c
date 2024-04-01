#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#if defined AF_INET || defined AF_INET6
#include <netinet/in.h>
#endif

#include "systemd/sd-daemon.h"

int sd_is_socket_sockaddr(int fd, int type,
                          const struct sockaddr *addr, size_t addrlen,
                          int listening)
{
	union {
		struct sockaddr sa;
		struct sockaddr_un un;
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
	if (!addr)
		return 1;

	if (getsockname(fd, &u.sa, &len) < 0)
		return errno != EOPNOTSUPP ? -errno : 0;
	assert(len >= offsetof(struct sockaddr, sa_family) + sizeof u.sa.sa_family);
	if (addrlen < offsetof(struct sockaddr, sa_family) + sizeof addr->sa_family)
		return -EINVAL;
	if (u.sa.sa_family != addr->sa_family)
		return 0;

	switch (addr->sa_family) {
	case AF_UNIX: {
		struct sockaddr_un *un;

		assert(len >= offsetof(struct sockaddr_un, sun_path));
		len -= offsetof(struct sockaddr_un, sun_path);
		assert(len <= sizeof u.un.sun_path);

		if (addrlen < offsetof(struct sockaddr_un, sun_path))
			return -EINVAL;
		addrlen -= offsetof(struct sockaddr_un, sun_path);
		if (addrlen > sizeof un->sun_path)
			return -EINVAL;

		un = (struct sockaddr_un *)addr;
		if (un->sun_path[0]) {
			const char *end;
			if ((end = memchr(un->sun_path, '\0', addrlen)))
				addrlen = end - un->sun_path;
		} else {
			static const char zero[sizeof un->sun_path];
			if (!memcmp(un->sun_path, zero, addrlen))
				addrlen = 0;
		}

		if (len != addrlen)
			return 0;
		return !memcmp(u.un.sun_path, un->sun_path, addrlen);
	}
#ifdef AF_INET
	case AF_INET: {
		struct sockaddr_in *in;
		assert(len >= sizeof u.in);
		if (addrlen < sizeof *in)
			return -EINVAL;
		in = (struct sockaddr_in *)addr;
		if (in->sin_port && u.in.sin_port != in->sin_port)
			return 0;
		return u.in.sin_addr.s_addr == in->sin_addr.s_addr;
	}
#endif
#ifdef AF_INET6
	case AF_INET6: {
		struct sockaddr_in6 *in6;
		assert(len >= sizeof u.in6);
		if (addrlen < sizeof *in6)
			return -EINVAL;
		in6 = (struct sockaddr_in6 *)addr;
		if (in6->sin6_port && u.in6.sin6_port != in6->sin6_port)
			return 0;
		if (in6->sin6_flowinfo && u.in6.sin6_flowinfo != in6->sin6_flowinfo)
			return 0;
		if (in6->sin6_scope_id && u.in6.sin6_scope_id != in6->sin6_scope_id)
			return 0;
		return !memcmp(&u.in6.sin6_addr.s6_addr,
		               &in6->sin6_addr.s6_addr,
		               sizeof in6->sin6_addr.s6_addr);
	}
#endif
	default:
		return -EINVAL;
	}
}
