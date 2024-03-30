#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#ifdef AF_VSOCK
#include <linux/vm_sockets.h>
#endif

#include "systemd/sd-daemon.h"

#define NOTIFY_SOCKET "NOTIFY_SOCKET"

#define STARTSWITH(PTR, LIT) \
	(!strncmp((PTR), (LIT), sizeof(LIT) - 1) ? sizeof(LIT) - 1 : 0)
#define EQUALS(PTR, END, LIT) \
	((END) - (PTR) == sizeof(LIT) - 1 && !memcmp((PTR), (LIT), sizeof(LIT) - 1))

int sd_notify_socket(int unset) {
	const char *addr;
	size_t len;
	union {
		struct sockaddr sa;
		struct sockaddr_un un;
#ifdef AF_VSOCK
		struct sockaddr_vm vm;
#endif
	} u;
	socklen_t addrlen;
	int fd, r;

	memset(&u, 0, sizeof u);

	addr = getenv(NOTIFY_SOCKET);
	if (!addr)
		return -EINVAL; /* FIXME */

	if (addr[0] == '/' || addr[0] == '@') {
		addrlen = sizeof u.un;
		u.un.sun_family = AF_UNIX;
		len = strlen(addr);
		if (len > sizeof u.un.sun_path) {
			r = -EINVAL; goto bail;
		}
		memcpy(u.un.sun_path, addr, len);
		if (addr[0] == '@') {
			u.un.sun_path[0] = '\0';
			addrlen = offsetof(struct sockaddr_un, sun_path) + len;
		}

		fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	} else {
#ifdef AF_VSOCK
		size_t start;
		int type;
		const char *colon;
		unsigned long tmp;
		char *end;

		if ((start = STARTSWITH(addr, "vsock:"))) {
			type = 0;
		} else if ((start = STARTSWITH(addr, "vsock-dgram:"))) {
			type = SOCK_DGRAM;
		} else if ((start = STARTSWITH(addr, "vsock-seqpacket:"))) {
			type = SOCK_SEQPACKET;
		} else if ((start = STARTSWITH(addr, "vsock-stream:"))) {
			type = SOCK_STREAM;
		} else {
			r = -EPROTO; goto bail;
		}
		addr += start;

		addrlen = sizeof(u.vm);
		u.vm.svm_family = AF_VSOCK;

		if (!(colon = strchr(addr, ':')) || colon == addr || !colon[1]) {
			r = -EINVAL; goto bail;
		}

		if (EQUALS(addr, colon, "hypervisor")) {
			u.vm.svm_cid = VMADDR_CID_HYPERVISOR;
		} else if (EQUALS(addr, colon, "local")) {
			u.vm.svm_cid = VMADDR_CID_LOCAL;
		} else if (EQUALS(addr, colon, "host")) {
			u.vm.svm_cid = VMADDR_CID_HOST;
		} else {
			errno = 0;
			tmp = strtoul(addr, &end, 0);
			if (end != colon) {
				r = -EINVAL; goto bail;
			}
			if (tmp > UINT_MAX || (tmp == ULONG_MAX && errno == ERANGE)) {
				r = -ERANGE; goto bail;
			}
			u.vm.svm_cid = tmp;
		}

		errno = 0;
		tmp = strtoul(colon + 1, &end, 0);
		if (*end) {
			r = -EINVAL; goto bail;
		}
		if (tmp > UINT_MAX || (tmp == ULONG_MAX && errno == ERANGE)) {
			r = -EINVAL; goto bail;
		}
		u.vm.svm_port = tmp;

		if (type) {
			fd = socket(PF_VSOCK, type, 0);
		} else do {
			if ((fd = socket(PF_VSOCK, SOCK_DGRAM, 0)) >= 0)
				break;
			if ((fd = socket(PF_VSOCK, SOCK_SEQPACKET, 0)) >= 0)
				break;
			if ((fd = socket(PF_VSOCK, SOCK_STREAM, 0)) >= 0)
				break;
		} while (0);
#else /* AF_VSOCK */
		r = -EPROTO; goto bail;
#endif /* AF_VSOCK */
	}

	if (fd < 0) {
		r = -errno; goto bail;
	}
	if (connect(fd, &u.sa, addrlen) < 0) {
		r = -errno; close(fd); goto bail;
	}
	r = fd;

bail:
	if (unset)
		unsetenv(NOTIFY_SOCKET);
	return r;
}
