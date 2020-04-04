# $OpenBSD: Makefile,v 1.17 2017/07/19 12:50:32 espie Exp $

PROG=		iked
SRCS=		ca.c chap_ms.c config.c control.c crypto.c dh.c \
		eap.c iked.c ikev2.c ikev2_msg.c ikev2_pld.c \
		log.c ocsp.c pfkey.c policy.c proc.c timer.c util.c \
		imsg_util.c smult_curve25519_ref.c
SRCS+=		eap_map.c ikev2_map.c
SRCS+=		parse.y
MAN=		iked.conf.5 iked.8
#NOMAN=		yes

LDADD=		-lutil -levent -lcrypto
DPADD=		${LIBUTIL} ${LIBEVENT} ${LIBCRYPTO}
CFLAGS+=	-Wall -I${.CURDIR}
CFLAGS+=	-Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare
CLEANFILES+=	ikev2_map.c eap_map.c
GENERATED=	ikev2_map.c eap_map.c

YFLAGS=

ikev2_map.c: genmap.sh ikev2.h
	/bin/sh ${.CURDIR}/genmap.sh ${.CURDIR}/ikev2.h ikev2 > $@
	@touch $@

eap_map.c: genmap.sh eap.h
	/bin/sh ${.CURDIR}/genmap.sh ${.CURDIR}/eap.h eap > $@
	@touch $@

# ------------------------------------------------------------------------------
# HACK

# ------------------------------------------------------------------------------

.if ${:!uname -s!} == "Linux"

all:
	echo '\e[32mall\e[0m'

	rsync -va -e 'ssh -p10722' -h --progress *.o root@localhost:/mfs/iked/
	# rsync -va -e 'ssh -p10722' -h --progress ./ root@localhost:/mfs/iked/

	# ssh -p10722 root@localhost 'cd /mfs/iked/ && clang -o iked ca.o chap_ms.o config.o control.o crypto.o dh.o eap.o iked.o ikev2.o ikev2_msg.o ikev2_pld.o log.o ocsp.o pfkey.o policy.o proc.o timer.o util.o imsg_util.o smult_curve25519_ref.o eap_map.o ikev2_map.o parse.o -lutil -levent -lcrypto'

	ssh -p10722 root@localhost 'cd /mfs/iked/ && cc -no-pie -o iked *.o -lutil -levent -lcrypto -lstdc++'
	scp -P10722 root@localhost:/mfs/iked/iked ./
	scp -P10622 ./iked root@localhost:/mfs/iked/iked
	echo '\e[32mall done\e[0m'

.endif  # .if ${:!uname -s!} == "Linux"

# ------------------------------------------------------------------------------
# tmp

# LDFLAGS+=	-fuse-ld=lld
# LDFLAGS+=	-fuse-ld=${HOME}/qc/binutils-gdb/build/ld/ld-new

# LDFLAGS+=	${HOME}/qc/openbsd-sysroot/usr/lib/crt0.o
# LDFLAGS+=	-L${HOME}/qc/openbsd-sysroot/usr/lib
# LDFLAGS+=	-R${HOME}/qc/openbsd-sysroot/usr/lib

# crt0.o
# crtbegin.o
# crtbeginS.o
# crtend.o
# crtendS.o
# gcrt0.o
# libcompiler_rt.a
# rcrt0.o

# ------------------------------------------------------------------------------

.include <bsd.prog.mk>

# Don't compile iked as static binary by default
LDSTATIC=
