//===-- wutils.hpp - wataash's single header library  -----------*- C++ -*-===//
//
// License: MIT
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Coding style follows "LLVM Coding Standards"
/// https://llvm.org/docs/CodingStandards.html
///
/// -include path/to/wutils.hpp
/// c++ -DWUTILS_IMPL path/to/wutils.hpp
///
///
/// ln -s wutils.cc wutils.cc.compile.cc
/// TODO: wutils.cc -> wutils.h
///
//===----------------------------------------------------------------------===//

#ifndef WUTILS_CC
#define WUTILS_CC

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <stdarg.h>

void w_sandbox(void);

// logging

void w_vl(const char *func, const char *format, va_list ap);
void w_l(const char *func, const char *format, ...)
    __attribute__((__format__(printf, 2, 3)));
void w_lw(const char *func, const char *format, ...)
    __attribute__((__format__(printf, 2, 3)));
void w_li(const char *func, const char *format, ...)
    __attribute__((__format__(printf, 2, 3)));
void w_ld(const char *func, const char *format, ...)
    __attribute__((__format__(printf, 2, 3)));

#define w_w(format, ...) w_lw(__func__, format, ##__VA_ARGS__)
#define w_i(format, ...) w_li(__func__, format, ##__VA_ARGS__)
#define w_d(format, ...) w_ld(__func__, format, ##__VA_ARGS__)

// misc

void w_lsof(void);
void w_print_colors(void);
void w_stop(void);
void w_systemf(const char *format, ...)
    __attribute__((__format__(printf, 1, 2))); // should be restrict?

// draft

void w_pprintf(const char *func, const char *format, ...)
    __attribute__((__format__(printf, 2, 3)));
char *w_sprintf3(const char *format, ...)
    __attribute__((__format__(printf, 1, 2)));
void w_loop_(const char *func);
void w_loop10_(const char *func);
#define w_loop()                                                               \
  do {                                                                         \
    w_i("");                                                                   \
    w_loop_(__func__);                                                         \
    w_i("return");                                                             \
  } while (0)
#define w_loop10()                                                             \
  do {                                                                         \
    w_i("");                                                                   \
    w_loop10_(__func__);                                                       \
    w_i("return");                                                             \
  } while (0)

unsigned long w_hash(const char *str);

__END_DECLS

#endif // WUTILS_CC

//===----------------------------------------------------------------------===//
// impl
//===----------------------------------------------------------------------===//

// TODO: move thils top
// 1.
// compile me with c++ compiler and linkme, with -DWUTILS_IMPL
// any other file must not -DWUTILS_IMPL
// 2.
// in a single exixting c++ source:
// #define WUTILS_IMPL
// #include wutils.cc
// 3.
//
// for C++ project (or C/C++ both project), in a single C++ file:
// #define WUTILS_IMPL
// #include wutils.cc
//
// for C project:
// compile and link me;
// * WUTILS_IMPL only for this file
// * all file flags: WUTILS_IMPL_IF_CXX
// * CXXFLAGS: DWUTILS_IMPL

// string comparison is so hard... https://stackoverflow.com/q/2335888/4085441
// // __BASE_FILE__ for clang; TODO: gcc
// #if __BASE_FILE__ == ../wutils/wutils.cc.compile.cc

#ifdef WUTILS_IMPL_IF_CXX
#ifdef __cplusplus
#pragma message "defining WUTILS_IMPL..."
#define WUTILS_IMPL
#endif // __cplusplus
#endif // WUTILS_IMPL_IF_CXX

#ifdef WUTILS_IMPL
#pragma message "compiling wutils impl"

#ifndef __cplusplus
#error C++ compiler is needed! (trying to compile with C compiler?)
#endif // __cplusplus

#include <string.h>

// c++
#include <csignal>
#include <cstdarg>
#include <cstring>
#include <iostream>

// c
#include <stdio.h>
#include <unistd.h>

namespace nswutils {

class CWUtils {
public:
  int iii;

  explicit CWUtils() { iii = 0; }
  explicit CWUtils(int i) { iii = i; }
  ~CWUtils() { iii = -1; }

  void lsof() { _lsof(); }

protected:
  void foo();

private:
  void _lsof();
};

void CWUtils::_lsof() { std::system("true"); }

} // namespace nswutils

//===----------------------------------------------------------------------===//
// C impl
//===----------------------------------------------------------------------===//

__BEGIN_DECLS
namespace {

void w_sandbox() {
  auto cp = new nswutils::CWUtils(); // size: 8
  auto c = nswutils::CWUtils(9);     // size: 4

  (void)cp->iii;
  (void)c.iii;

  c.lsof();

  delete (cp);
  cp = nullptr;
}

// -----------------------------------------------
// logging

/// w_vl(__func__, "%s", ap)
/// => >>> XXXXX __func__ ap
///        ^^^^^ PID
void w_vl(const char *func, const char *format, va_list ap) {
  std::cout << ">>> " << getpid() << ' ' << func << ' ';
  std::vprintf(format, ap);
  std::cout << std::endl;
}

/// w_l(__func__, "%s", "foo")
/// => XXXXX __func__ foo
void w_l(const char *func, const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  w_vl(func, format, ap);

  va_end(ap);
}

static void log_color(const char *func, const char *format, va_list ap,
                      const char *color) {
  std::cout << ">>> " << color << getpid() << ' ' << func << ' ';
  std::vprintf(format, ap);
  std::cout << "\x1b[0m" << std::endl;
}

void w_lw(const char *func, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  ::log_color(func, format, ap, "\x1b[33m");
  va_end(ap);
}

void w_li(const char *func, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  ::log_color(func, format, ap, "\x1b[36m");
  va_end(ap);
}

void w_ld(const char *func, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  ::log_color(func, format, ap, "\x1b[37m");
  va_end(ap);
}

// -----------------------------------------------
// misc

void w_lsof() {
  // exclude:
  // COMMAND     PID USER   FD   TYPE DEVICE SIZE/OFF     NODE NAME
  // lib_libev 31937  wsh  mem    REG  253,1  1365096 36315339
  // /usr/lib/x86_64-linux-gnu/libm-2.29.so
  w_systemf("lsof -p %jd | ag -v /usr/lib/x86_64", (std::intmax_t)getpid());
}

void w_print_colors() {
  std::cout << "\x1b[30m black   \x1b[0m" << std::endl;
  std::cout << "\x1b[31m red     \x1b[0m" << std::endl;
  std::cout << "\x1b[32m green   \x1b[0m" << std::endl;
  std::cout << "\x1b[33m yellow  \x1b[0m" << std::endl;
  std::cout << "\x1b[34m blue    \x1b[0m" << std::endl;
  std::cout << "\x1b[35m magenta \x1b[0m" << std::endl;
  std::cout << "\x1b[36m cyan    \x1b[0m" << std::endl;
  std::cout << "\x1b[37m white   \x1b[0m" << std::endl;
}

void w_systemf(const char *format, ...) {
  char buf[100];
  va_list ap;

  va_start(ap, format);
  int len = std::vsnprintf(buf, sizeof(buf), format, ap);
  (void)len; // XXX: len vs sizeof(buf) not checked
  va_end(ap);

  std::printf("\x1b[36m system %s \x1b[37m\n", buf);

  int err_ = std::system(buf);
  if (err_ != 0) {
    perror("system");
  }

  std::printf("\x1b[0m");
}

void w_stop() {
  // TODO: pthread_kill?
  w_w("SIGSTOP");
  kill(getpid(), SIGSTOP);
  w_w("SIGSTOP done");
}

// -----------------------------------------------
// draft

// w_pprintf(__func__, "%s %s\n", "foo", "bar")
// to be
// w_pprintf(__func__, "%s %s\n", "foo", "bar");
void w_pprintf(const char *func, const char *format, ...) {
  char new_fmt[1024];
  va_list ap;

  // ">>> 01234, main()"
  int len =
      snprintf(new_fmt, sizeof(new_fmt), ">>> %05d, %s() ", getpid(), func);
  (void)len; // XXX: len vs sizeof(new_fmt) not checked

  // ">>> 01234, main()" + "%s %s\n"
  len = strlcat(new_fmt, format, sizeof(new_fmt));
  (void)len; // XXX: len vs sizeof(new_fmt) not checked

  va_start(ap, format);
  vprintf(new_fmt, ap);
  va_end(ap);

  len = 0; // breakpoint
}

char *w_sprintf3(const char *format, ...) {
  static char bufs[3][10240]; // 10KB * 3
  static unsigned int current_buf = 0;
  va_list ap;

  va_start(ap, format);
  int len = vsnprintf(bufs[current_buf], sizeof(bufs[0]), format, ap);
  (void)len; // TODO
  va_end(ap);

  char *ret = bufs[current_buf];
  current_buf = (current_buf + 1) % 3;
  return ret;
}

void w_loop_(const char *func) {
  static volatile int not_loop = 0; // set me to 1 with gdb
  if (not_loop)
    return;

  volatile long break_ = 1;
  while (break_ != 0) {
    // to quit: wait ~1s or `break_ = 0` with gdb
    break_++;
    // if (break_ == (long)1e9) { // tune me
    if (break_ == 5e8) {
      break;
    }
  }
  return; // breakpoint
}

void w_loop10_(const char *func) {
  volatile long break_ = 1;
  while (break_ != 0) {
    break_++;
    if (break_ == 1e10) {
      break;
    }
  }
  return; // breakpoint
}

// https://stackoverflow.com/a/7666577/4085441
unsigned long w_hash(const char *str) {
  unsigned long hash = 5381;
  // "a" -> 177670
  // "b" -> 177671
  // "c" -> 177672

  // static unsigned long hash = 5381;

  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

// -----------------------------------------------

} // namespace
__END_DECLS

// -----------------------------------------------------------------------------
// TODO merge

// 		void *fa0 =__builtin_frame_address(0);
// 		void *fa1 =__builtin_frame_address(1);
// 		void *fa2 =__builtin_frame_address(2);
//
// 		void *ra0 =__builtin_return_address(0);
// 		void *ra1 =__builtin_return_address(1);
// 		void *ra2 =__builtin_return_address(2);
//
// 		char *fmt2;
// 		asprintf(
// 		    &fmt2,
// 		    "fa0: %p, fa1: %p, fa2: %p, ra0: %p, ra1: %p, ra2: %p, %s",
// 		    fa0, fa1, fa2, ra0, ra1, ra2, fmt);
// 		if (fmt2 == NULL)
// 			abort();
//
// 		fmt = fmt2;
//
// free(fmt2);

#endif // WUTILS_IMPL
