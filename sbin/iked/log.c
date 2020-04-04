/*	$OpenBSD: log.c,v 1.12 2017/03/21 12:06:55 bluhm Exp $	*/

/*
 * Copyright (c) 2003, 2004 Henning Brauer <henning@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>

#ifdef DEBUG_LOG_PRETTY
#include <unistd.h>
#include "iked.h"
#undef log_procinit
#undef log_warn
#undef log_warnx
#undef log_info
#undef log_debug
#endif /* DEBUG_LOG_PRETTY */

static int	 debug;
static int	 verbose;
const char	*log_procname;

void	log_init(int, int);
void	log_procinit(const char *);
void	log_setverbose(int);
int	log_getverbose(void);
void	log_warn(const char *, ...)
	    __attribute__((__format__ (printf, 1, 2)));
void	log_warnx(const char *, ...)
	    __attribute__((__format__ (printf, 1, 2)));
void	log_info(const char *, ...)
	    __attribute__((__format__ (printf, 1, 2)));
void	log_debug(const char *, ...)
	    __attribute__((__format__ (printf, 1, 2)));
void	logit(int, const char *, ...)
	    __attribute__((__format__ (printf, 2, 3)));
void	vlog(int, const char *, va_list)
	    __attribute__((__format__ (printf, 2, 0)));
__dead void fatal(const char *, ...)
	    __attribute__((__format__ (printf, 1, 2)));
__dead void fatalx(const char *, ...)
	    __attribute__((__format__ (printf, 1, 2)));

void
log_init(int n_debug, int facility)
{
	extern char	*__progname;

	debug = n_debug;
	verbose = n_debug;
	log_procinit(__progname);

	if (!debug)
		openlog(__progname, LOG_PID | LOG_NDELAY, facility);

	tzset();
}

void
log_procinit(const char *procname)
{
	if (procname != NULL)
		log_procname = procname;
}

void
log_setverbose(int v)
{
	verbose = v;
}

int
log_getverbose(void)
{
	return (verbose);
}

void
logit(int pri, const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vlog(pri, fmt, ap);
	va_end(ap);
}

void
vlog(int pri, const char *fmt, va_list ap)
{
	char	*nfmt;
	int	 saved_errno = errno;

	if (debug) {
		/* best effort in out of mem situations */
		if (asprintf(&nfmt, "%s\n", fmt) == -1) {
			vfprintf(stderr, fmt, ap);
			fprintf(stderr, "\n");
		} else {
			vfprintf(stderr, nfmt, ap);
			free(nfmt);
		}
		fflush(stderr);
	} else
		vsyslog(pri, fmt, ap);

	errno = saved_errno;
}

void
log_warn(const char *emsg, ...)
{
	char		*nfmt;
	va_list		 ap;
	int		 saved_errno = errno;

	/* best effort to even work in out of memory situations */
	if (emsg == NULL)
		logit(LOG_ERR, "%s", strerror(saved_errno));
	else {
		va_start(ap, emsg);

		if (asprintf(&nfmt, "%s: %s", emsg,
		    strerror(saved_errno)) == -1) {
			/* we tried it... */
			vlog(LOG_ERR, emsg, ap);
			logit(LOG_ERR, "%s", strerror(saved_errno));
		} else {
			vlog(LOG_ERR, nfmt, ap);
			free(nfmt);
		}
		va_end(ap);
	}

	errno = saved_errno;
}

void
log_warnx(const char *emsg, ...)
{
	va_list	 ap;

	va_start(ap, emsg);
	vlog(LOG_ERR, emsg, ap);
	va_end(ap);
}

void
log_info(const char *emsg, ...)
{
	va_list	 ap;

	va_start(ap, emsg);
	vlog(LOG_INFO, emsg, ap);
	va_end(ap);
}

void
log_debug(const char *emsg, ...)
{
	va_list	 ap;

	if (verbose > 1) {
		va_start(ap, emsg);
		vlog(LOG_DEBUG, emsg, ap);
		va_end(ap);
	}
}

static void
vfatalc(int code, const char *emsg, va_list ap)
{
	static char	s[BUFSIZ];
	const char	*sep;

	if (emsg != NULL) {
		(void)vsnprintf(s, sizeof(s), emsg, ap);
		sep = ": ";
	} else {
		s[0] = '\0';
		sep = "";
	}
	if (code)
		logit(LOG_CRIT, "%s: %s%s%s",
		    log_procname, s, sep, strerror(code));
	else
		logit(LOG_CRIT, "%s%s%s", log_procname, sep, s);
}

void
fatal(const char *emsg, ...)
{
	va_list	ap;

	va_start(ap, emsg);
	vfatalc(errno, emsg, ap);
	va_end(ap);
	exit(1);
}

void
fatalx(const char *emsg, ...)
{
	va_list	ap;

	va_start(ap, emsg);
	vfatalc(0, emsg, ap);
	va_end(ap);
	exit(1);
}

#ifdef DEBUG_LOG_PRETTY

static pid_t 		 static_pid;
static unsigned int	 color256 = 143; // calculated for "parent"

void
log_procinit_pretty(const char *procname)
{
	log_procinit(procname);
	if (procname != NULL)
		log_procname = procname;
	static_pid = getpid();
	unsigned long hash_procname = w_hash(log_procname);
	color256 = hash_procname % 256U;

	// usleep(color256 * 1000); // 0-255 ms to avoid logging crashing

	// avoid logging crashing
	if (strcmp(procname, "parent") == 0)
		usleep(0);
	else if (strcmp(procname, "control") == 0)
		usleep(100000); // 100ms
	else if (strcmp(procname, "ca") == 0)
		usleep(200000);
	else if (strcmp(procname, "ikev2") == 0)
		usleep(300000);
	else
		fatalx("unknown procname: %s", procname);

	_mylog(LOG_FUNC_INFO, __FILE__, __LINE__, __func__, "initialized %s %u", log_procname, color256);
}

// has ("%s", __func__))
static unsigned int
has_s_func(const char *func, const char *format, va_list ap)
{
	if (strncmp(format, "%s", 2) != 0)
		return 0;
	const char *const s = va_arg(ap, const char *);
	if (strcmp(s, func) != 0)
		return 0;
	return 1;
}

void
_mylog(enum log_func lfunc, const char *file, unsigned int line, const char *func, const char *format, ...)
{
	// TODO: stderr -> string
	// TODO: profiling

	va_list ap;

	// 15:04:05.000
	{
		char buf_time[20];
		struct timespec ts_now;
		clock_gettime(CLOCK_REALTIME, &ts_now);
		struct tm tm_local;
		localtime_r(&ts_now.tv_sec, &tm_local);
		// 15:29.14.123
		size_t todo = strftime(buf_time, sizeof(buf_time), "%T", &tm_local);
		(void)todo;
		// TODO: intmax_t? or uintmax_t?
		// TODO: stderr -> buf
		fprintf(stderr, "%s.%03jd", buf_time, (intmax_t)(ts_now.tv_nsec / 1000000));
	}

	// 15:04:05.000  2345
	// 15:04:05.000 12345
	//              ^^^^^ color by procname
	fprintf(stderr, " \x1b[38;5;%um%5jd\x1b[0m", color256, (intmax_t)static_pid);

	// 15:04:05.000 12345          pfkey_reply hdr.sadb_msg_len: 31
	// 15:04:05.000 12345 [WARN] pfkey_process reached
	//                    |-> color
	//                    \___________________/ %20s
	switch (lfunc) {
	case LOG_FUNC_WARN:
	case LOG_FUNC_WARNX:
		// fprintf(stderr, " \x1b[33mWARN\x1b[0m");
		// fprintf(stderr, " \x1b[33mWARN");
		fprintf(stderr, " \x1b[33m[WARN]");
		fprintf(stderr, "%14s", func);
		break;
	case LOG_FUNC_INFO:
		// fprintf(stderr, " \x1b[34mINFO\x1b[0m");
		// fprintf(stderr, " \x1b[34mINFO");
		fprintf(stderr, " \x1b[34m");
		fprintf(stderr, "%20s", func);
		break;
	case LOG_FUNC_DEBUG:
	case LOG_FUNC_TRACE:
		// fprintf(stderr, " \x1b[37mDEBU\x1b[0m");
		// fprintf(stderr, " \x1b[37mDEBU");
		fprintf(stderr, " \x1b[37m");
		fprintf(stderr, "%20s", func);
		break;
	default:
		/* NOTREACHED */
		fprintf(stderr, " \x1b[31m[NOTREACHED]");
		fprintf(stderr, "%8s", func);
		break;
	}

	// 15:04:05.000 12345 INFO          func message 0
	//
	// remove explicit __func__
	// ("%s: message %d", __func__, 0)
	// -> (": message %d", 0)

	va_start(ap, format);
	unsigned int _has_s_func = has_s_func(func, format, ap);
	va_end(ap);
	if (_has_s_func) {
		// format = sprintf3("%s %s", func, format);
	}

	char buf[10240];
	const char *debug_vizualize_buf = buf;
	(void)debug_vizualize_buf;

	// loop_nolog();

	{
		va_start(ap, format);
		if (_has_s_func) {
			// remove __func__
			const char *_func = va_arg(ap, const char *);
			(void)_func;
			format += 2; // remove %s
		}
		// int todo = vsnprintf_ss(buf, sizeof(buf), format, ap);
		int todo = vsprintf(buf, format, ap);
		(void)todo;
		todo = 0; // breakpoint
		(void)todo;
	}
	va_end(ap);

	switch (lfunc) {
	case LOG_FUNC_WARN:
		log_warn(" %s", buf);
		break;
	case LOG_FUNC_WARNX:
		log_warnx(" %s", buf);
		break;
	case LOG_FUNC_INFO:
		log_info(" %s", buf);
		break;
	case LOG_FUNC_DEBUG:
		log_debug(" %s", buf);
		break;
	default:
		fatalx(" NOTREACHED lfunc:%d buf:%s", lfunc, buf);
		break;
	}

	fprintf(stderr, "\x1b[0m");
}

#endif /* DEBUG_LOG_PRETTY */
