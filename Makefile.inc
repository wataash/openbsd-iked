# build on ubuntu

# probably this syntax doesn't work on OpenBSD's make
.if ${:!uname -s!} == "Linux"

CFLAGS+=	-fno-pie
CFLAGS+=	-O0 -ggdb3

LDFLAGS+=	-fuse-ld=/bin/true  # cross ld is not easy...
# LDFLAGS+=	-fuse-ld=/bin/echo

# SRCS+=		wutils.cc.compile.cc
SRCS+=		wutils.cc
.PATH:		${.CURDIR}/../../wutils
CPPFLAGS+=	-DWUTILS_IMPL_IF_CXX

CPPFLAGS+=	-DDEBUG_EVENT
CPPFLAGS+=	-DDEBUG_INSECURE
CPPFLAGS+=	-DDEBUG_LOG_PRETTY
CPPFLAGS+=	-DNO_SETPGID  # to fasten ctrl-C
# CPPFLAGS+=	-include ../wutils/wutils.cc

# TODO
# clang++ can't search headers with -idirafter ?
CXXFLAGS+=	-I /home/wsh/qc/openbsd-sysroot/usr/include/c++/v1
CXXFLAGS+=	-nostdinc++
CXXFLAGS+=	-cxx-isystem=/home/wsh/qc/openbsd-sysroot/c++/v1
CXXFLAGS+=	--sysroot=/home/wsh/qc/openbsd-sysroot

LDSTATIC=	-static

.endif  # .if ${:!uname -s!} == "Linux"
