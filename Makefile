CPPFLAGS = -D_GNU_SOURCE -Wall -Wpedantic
CFLAGS = -g -O2 -std=c89

OBJS = \
	sd_notify.o \
	sd_notifyf.o \
	sd_pid_notify.o \
	sd_pid_notifyf.o \
	sd_pid_notify_with_fds.o \
	sd_pid_notifyf_with_fds.o \
	sd_notify_barrier.o \
	sd_pid_notify_barrier.o \
	# OBJS

all: $(OBJS)
clean: ; rm -f $(OBJS)
$(OBJS): systemd/sd-daemon.h
sd_notifyf.o sd_pid_notifyf.o sd_pid_notifyf_with_fds.o: sd_notifyf_internal.h
