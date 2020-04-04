# $OpenBSD: Makefile,v 1.7 2016/09/11 14:31:02 natano Exp $

.PATH:		${.CURDIR}/../../sbin/iked

PROG=		ikectl
SRCS=		log.c ikeca.c ikectl.c parser.c util.c

MAN=		ikectl.8

LDADD=		-lutil -lcrypto
DPADD=		${LIBUTIL} ${LIBCRYPTO}
CFLAGS+=	-Wall -I${.CURDIR} -I${.CURDIR}/../../sbin/iked
CFLAGS+=	-Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare

distribution:
	${INSTALL} -C -o root -g wheel -m 0644 ${.CURDIR}/ikeca.cnf \
	    ${DESTDIR}/etc/ssl/ikeca.cnf

# ------------------------------------------------------------------------------
# HACK

# ------------------------------------------------------------------------------
# build on ubuntu

.if ${:!uname -s!} == "Linux"

all:
	echo '\e[32mall\e[0m'

	rsync -va -e 'ssh -p10722' -h --progress *.o root@localhost:/mfs/ikectl/

	ssh -p10722 root@localhost 'cd /mfs/ikectl/ && cc -no-pie -o ikectl *.o -lutil -lcrypto -lstdc++'
	scp -P10722 root@localhost:/mfs/ikectl/ikectl ./
	scp -P10622 ./ikectl root@localhost:/mfs/ikectl/ikectl
	echo '\e[32mall done\e[0m'

.endif  # .if ${:!uname -s!} == "Linux"

# ------------------------------------------------------------------------------
.include <bsd.prog.mk>
