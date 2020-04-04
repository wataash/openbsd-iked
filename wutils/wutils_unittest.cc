#include <tuple>

#include <gtest/gtest.h>

#include "wutils.cc"

namespace {

TEST(Suite0, logger) {
std::setbuf(stdout, nullptr);
// TODO: assertion
// testing::internal::CaptureStdout();
w_l(__func__, "foo %d", 9);
w_lw(__func__, "foo %d", 9);
w_li(__func__, "bar");
w_ld(__func__, "%s", "baz");

w_w("foo %d", 9);
w_i("bar");
w_d("%s", "baz");

// auto tmp = testing::internal::GetCapturedStdout();

// tmp = ""; // breakpoint
}

TEST(Suite0, sandbox) {
w_sandbox();
// ...
}

}  // namespace
