#ifndef SD_DAEMON_H
#define SD_DAEMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

int sd_notify(int, const char *);
int sd_notifyf(int, const char *, ...);
int sd_pid_notify(pid_t, int, const char *);
int sd_pid_notifyf(pid_t, int, const char *, ...);
int sd_pid_notify_with_fds(pid_t, int, const char *, const int *, size_t);
int sd_pid_notifyf_with_fds(pid_t, int, const int *, size_t, const char *, ...);

#ifdef __cplusplus
}
#endif

#endif
