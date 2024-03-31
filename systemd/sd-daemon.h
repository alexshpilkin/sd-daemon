#ifndef SD_DAEMON_H
#define SD_DAEMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/types.h>

/* sd-daemon(3) */

#define SD_EMERG   "<0>"
#define SD_ALERT   "<1>"
#define SD_CRIT    "<2>"
#define SD_ERR     "<3>"
#define SD_WARNING "<4>"
#define SD_NOTICE  "<5>"
#define SD_INFO    "<6>"
#define SD_DEBUG   "<7>"

/* sd_notify(3) */

int sd_notify(int, const char *);
int sd_notifyf(int, const char *, ...);
int sd_pid_notify(pid_t, int, const char *);
int sd_pid_notifyf(pid_t, int, const char *, ...);
int sd_pid_notify_with_fds(pid_t, int, const char *, const int *, size_t);
int sd_pid_notifyf_with_fds(pid_t, int, const int *, size_t, const char *, ...);

#ifdef UINT64_MAX
int sd_notify_barrier(int, uint64_t);
int sd_pid_notify_barrier(pid_t, int, uint64_t);
#endif

#ifdef __cplusplus
}
#endif

#endif
