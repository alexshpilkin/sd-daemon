#ifndef SD_DAEMON_H
#define SD_DAEMON_H

#ifdef __cplusplus
extern "C" {
#endif

int sd_notify_socket(int);

int sd_notify(int, const char *);
int sd_notifyf(int, const char *, ...);

#ifdef __cplusplus
}
#endif

#endif
