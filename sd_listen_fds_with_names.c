#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "systemd/sd-daemon.h"
#include "sd_listen_fds_internal.h"

#define UNKNOWN "unknown"

int sd_listen_fds_with_names(int unset, char ***names) {
	const char *value;
	size_t i, count = 0;
	char **ar = NULL;
	int fds, r = 0;

	if (names && (value = getenv(LISTEN_FDNAMES))) {
		const char *name;

		for (name = value; name; name = strchr(name + 1, ':'))
			count++;
		if (!(ar = calloc(count + 1, sizeof *ar))) {
			r = -errno; goto bailearly;
		}

		name = value;
		for (i = 0; i < count; i++) {
			const char *next;
			next = strchr(name, ':');
			if (!next)
				next = name + strlen(name);
			if (!(ar[i] = malloc(next - name + 1))) {
				r = -errno; goto bailearly;
			}
			memcpy(ar[i], name, next - name);
			ar[i][next - name] = '\0';
			name = next + 1;
		}
	}

bailearly: /* call sd_listen_fds to unset variables even if we're bailing */

	fds = sd_listen_fds(unset);
	if (fds < 0)
		r = fds;
	if (r < 0)
		goto bail;

	if (count && count != (unsigned)fds) {
		r = -EINVAL; goto bail;
	}
	if (!count && fds && names) {
		if (!(ar = calloc(fds + 1, sizeof *ar))) {
			r = errno; goto bail;
		}
		for (i = 0; i < (unsigned)fds; i++) {
			if (!(ar[i] = malloc(sizeof UNKNOWN))) {
				r = errno; goto bail;
			}
			memcpy(ar[i], UNKNOWN, sizeof UNKNOWN);
		}
	}
	r = fds;

bail:
	if (names)
		*names = ar;
	if (r <= 0) {
		for (i = 0; i < count; i++)
			free(ar[i]);
		free(ar);
	}
	return r;
}
