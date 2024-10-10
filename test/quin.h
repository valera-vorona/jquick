#ifndef _QUIN_H_
#define _QUIN_H_

#ifndef qu_printf
  #include <stdio.h>
  #define qu_printf printf
#endif

#ifndef TEST_WO_GLOBALS

int qu_total;
int qu_failed;

#define TEST(name) extern int name() { \
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

