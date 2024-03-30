CPPFLAGS = -Wall
CFLAGS = -g -O2

OBJS = \
	sd_notify.o \
	sd_notifyf.o \
	sd_notify_socket.o \
	# OBJS

all: $(OBJS)
clean: ; rm -f $(OBJS)
$(OBJS): systemd/sd-daemon.h
