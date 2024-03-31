CPPFLAGS = -D_GNU_SOURCE -DNDEBUG -Wall -Wpedantic
CFLAGS = -g -O2 -std=c89

OBJS = \
	sd_booted.o \
	sd_is_fifo.o \
	sd_is_socket.o \
	sd_is_socket_inet.o \
	sd_is_socket_unix.o \
	sd_listen_fds.o \
	sd_listen_fds_with_names.o \
	sd_notify.o \
	sd_notifyf.o \
	sd_pid_notify.o \
	sd_pid_notifyf.o \
	sd_pid_notify_with_fds.o \
	sd_pid_notifyf_with_fds.o \
	sd_notify_barrier.o \
	sd_pid_notify_barrier.o \
	# OBJS

all: libsd-daemon.a
mostlyclean: ; rm -f $(OBJS)
clean: mostlyclean ; rm -f libsd-daemon.a
libsd-daemon.a: $(OBJS) ; $(AR) $(ARFLAGS) libsd-daemon.a $(OBJS)
$(OBJS): systemd/sd-daemon.h
sd_notifyf.o sd_pid_notifyf.o sd_pid_notifyf_with_fds.o: sd_notifyf_internal.h
sd_listen_fds.o sd_listen_fds_with_names.o: sd_listen_fds_internal.h
