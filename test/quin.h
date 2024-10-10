/*
///
/// # quin 
///
/// ## About
/// This is a minimal and quick unit test labrary for C and C++ programming
/// languages written in ANCI C and 
/// licensed under the MIT License <http://opensource.org/licenses/MIT>.
/// SPDX-License-Identifier: MIT
///

/// This library is header only and consists of only one header file. So
/// you can just copy it near to your code and use.

MIT License

Copyright (c) 2024 valera-vorona

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

/// ## Features
/// * Single header library
/// * It uses only printf function and has no other dependencies. If you want to use
///   your own print function. just redefine qu_printf
/// * No dynamic memory is allocated
///
/// ## Usage
/// You can just copy this file nearly to your code and include.
///
/// ## Example
/// ~~~
/// soon
/// ~~~
///
*/

#ifndef _QUIN_H_
#define _QUIN_H_

#ifndef qu_printf
  #include <stdio.h>
  #define qu_printf printf
#endif

#ifndef TEST_WO_GLOBALS

int qu_total;
int qu_failed;

#define TEST(name) int name() { \
  const char *test_name = #name; \
  qu_total = 0; \
  qu_failed = 0; \
  qu_printf("Begin quin test '" #name "'\n"); \
  qu_printf("--------------------------------\n");

#define TEST_END() \
  qu_printf("Overall test '%s' results:\n", test_name); \
  qu_printf("Total: %i, Passed: %i, Failed: %i\n", \
    qu_total, qu_total - qu_failed, qu_failed); \
  qu_failed ? qu_printf("%i TESTS FAILED\n", qu_failed) : qu_printf("ALL TESTS PASSED\n"); \
  return qu_failed; }

#define TEST_RUN(name) name()

#endif /* TEST_WO_GLOBALS */

#define TEST_SUITE(name) int name() { \
  int total = 0, failed = 0; \
  qu_printf("Begin test suite '" #name "'\n");

#ifndef TEST_WO_GLOBALS
  #define TEST_SUITE_END() \
    qu_total += total; \
    qu_failed += failed; \
    qu_printf("Total: %i, Passed: %i, Failed: %i\n", total, total - failed, failed); \
    failed ? qu_printf("%i SUITE TESTS FAILED\n", failed) : qu_printf("ALL SUITE TESTS PASSED\n"); \
    qu_printf("--------------------------------\n"); \
    return failed; }
#else
  #define TEST_SUITE_END() \
    qu_printf("Total: %i, Passed: %i, Failed: %i\n", total, total - failed, failed); \
    failed ? qu_printf("%i SUITE TESTS FAILED\n", failed) : qu_printf("ALL SUITE TESTS PASSED\n"); \
    qu_printf("--------------------------------\n"); \
    return failed; }
#endif

#define TEST_SUITE_RUN(name) name()

#define TEST_CASE(name) int name() {
#define TEST_CASE_END() return 0; }
#define TEST_CASE_RUN(test) do { \
  ++total; if (test()) { qu_printf("- Error in '" #test "'\n"); ++failed; } } while (0)

#define TEST_REQUIRE(test) do { if (!(test)) return 1; } while (0)

#endif /* _QUIN_H_ */

