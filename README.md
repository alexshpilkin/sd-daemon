# sd-daemon

<!-- SPDX-License-Identifer: CC0 OR 0BSD -->

This is `sd-daemon`, a reimplementation of the [sd-daemon(3)][] routines from
`libsystemd` as a portable static library for POSIX.1-2001 systems.  It is
not necessarily better than the original implementation (it is certainly
less well-tested), but it is more compact, is organized as a proper static
library with separately linkable objects, and only requires a POSIX libc
and a C89 or C++98 compiler to build.

## What is this for?

Talking to systemd if your program is a daemon running under it.

The `sd_notify_*()` and `sd_pid_notify_*()` functions notify systemd of the
state of your daemon (for example, that it is ready to process requests, so
its dependents can now be started).  The `sd_watchdog_enabled()` function tells
you if systemd is expecting periodic watchdog resets using `sd_notify*()`.

The `sd_listen_fds*()` functions can be used to identify and retrieve file
descriptors received in socket activation, and the `sd_is_*()` functions to
ascertain which kind of file descriptor you are looking at.

Finally, the `sd_booted()` function tells you if the system is running
systemd as the system-level service manager.

There is no detailed documentation available at the moment, but you can use
the systemd man page [sd-daemon(3)][] and its references instead.

[sd-daemon(3)]: https://www.freedesktop.org/software/systemd/man/latest/sd-daemon.html

## Building

Running `./configure && make && sudo make install` will install a a static
library, a header, and a [pkg-config][] file to `/usr/local`, with the usual
configuration options available.  If you don't like the configure script,
`make -f Makefile.in` will build a `libsd-daemon.a` in the source directory,
to be used alongside the header located in `systemd/sd-daemon.h`, so you only
lose the pkg-config support.  Finally, you can import the sources you need
into your project and build them any way you want, as long as you remember
to pass `-D_GNU_SOURCE` to get the Linux-specific VSOCK support.

[pkg-config]: https://www.freedesktop.org/wiki/Software/pkg-config

## Usage

In your source code, `#include <systemd/sd-daemon.h>` the same way you would
for the libsystemd implementation.  In your build system, include the output
of `pkg-config --cflags sd-daemon` in your compiler's command line and the
output of `pkg-config --libs sd-daemon` at the end of your linker's command
line if you installed the library; otherwise, arrange for the compiler to
find the include file as appropriate and put the `libsd-daemon.a` you built
at the end of the linker's command line with the other libraries.

## Compatibility

This library should be source-compatible with libsystemd release 255, except
for the following:

The functions `sd_is_mq()` and `sd_is_special()` are absent.  I am open
to introducing the former if someone turns out to care about using socket
activation with POSIX message queues, although it is fairly Linux-specific by
its nature.  The latter does not make any sense to me from the documentation;
I have looked at its implementation in `libsystemd` out of sheer desperation,
and turns out that does not make any sense either.

The functions `sd_is_socket_sockaddr()` and `sd_pid_notify_with_fds()` take
a `size_t` argument where the original takes an `unsigned` one.

No attempt is made to mark transient file descriptors `CLOEXEC`.

## Size

The source code currently weighs in at a bit under 800 lines, and the x86-64
machine code for the whole library is around 10K bytes with `gcc -O2`.  This is
more than I would really prefer, and the following reasons annoy me the most:

- Around 100 lines are in simple forwarder functions, like
  `sd_notify_barrier()` is for `sd_pid_notify_barrier()`.  This is due to
  each of those being its own source file, with includes and such, following
  standard practices for static libraries.

- Around 100 lines are spent in `sd_pid_notify_with_fds.c` on parsing the
  notification socket address, and most of those on the Linux VSOCK (virtio
  socket) support that ad hoc reimplementations in other projects don't
  bother with.

- The rest of that file, about 100 more lines, is spent on file descriptor
  and credential passing, because `sendmsg()` is that cumbersome.  Mainline
  systemd also has a pecularity where for setuid/setgid processes it will
  pass the real UID/GID instead of the effective one.  Reproducing that means
  that even the simplest notification functions like `sd_notify()`, which
  would otherwise be a glorified `send()`, need to forward to this code.
  This would not be needed if we did not care about setuid/setgid, but I do
  not see a clean way to make this an option.

Suggestions for improvements are welcome, as is evidence that some of the
pecularities mentioned above are not worth caring about.

## License

Public domain, or as close as we can get to it.  You can use either the [CC0][]
deed in `COPYING.CC0` in this distribution or the [0BSD][]/FPL license in
`COPYING.FPL`, at your option.  If you want to make a contribution to this
repository, you need to grant others the right to use it under those terms,
although you can always fork and relicense your own copy however you wish.

[CC0]: https://creativecommons.org/publicdomain/zero/1.0/
[0BSD]: https://opensource.org/license/0BSD

## Maintainership

This library was created in response to the [XZ backdoor][] and the
accompanying realization that multiple distros patch important daemons to
link to `libsystemd` just to be able to call `sd_notify(0, "READY=1")` once.
I hope the existence of this library will help with that a bit.

On the other hand, the XZ Utils project is a low-profile, basically feature
complete piece of foundational software maintained by a single guy, and this
library would be no different even if it does not need binary test inputs.
(Right now it has no tests at all!)  Moreover, you likely do not know or
trust me.

So I am, in no uncertain terms, giving this away.  If an IBM employee or
a Debian developer or even an Arch Linux TU wants to step up and maintain
this, I will be happy to turn over this leaf and go do something else.
My only aspiration is that this small bit of code will improve the situation.

Though maybe refrain from making it into a dynamic library?  Please?

[XZ backdoor]: https://tukaani.org/xz-backdoor/
