#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "utils.h"

// Mock state
jmp_buf jump_buffer;
bool exit_expected = false;

// Mock vFailureExit
void vFailureExit (bool bHelp, const char *format, ...) {
    va_list va;
    va_start (va, format);
    printf("Mock vFailureExit called: ");
    vprintf(format, va);
    printf("\n");
    va_end (va);

    if (exit_expected) {
        longjmp(jump_buffer, 1);
    } else {
        printf("Unexpected exit!\n");
        exit(1);
    }
}

// Test harness for iGetIntList
static void test_iGetIntList_simple() {
    int len = 0;
    int *list = iGetIntList("test", "1,2,3", &len);
    if (len != 3) {
        printf("FAIL: simple list length mismatch: %d != 3\n", len);
        exit(1);
    }
    if (list[0] != 1 || list[1] != 2 || list[2] != 3) {
        printf("FAIL: simple list content mismatch\n");
        exit(1);
    }
    free(list);
    printf("PASS: simple list\n");
}

static void test_iGetIntList_range() {
    int len = 0;
    int *list = iGetIntList("test", "1:3", &len);
    if (len != 3) {
        printf("FAIL: range list length mismatch: %d != 3\n", len);
        exit(1);
    }
    if (list[0] != 1 || list[1] != 2 || list[2] != 3) {
        printf("FAIL: range list content mismatch\n");
        exit(1);
    }
    free(list);
    printf("PASS: range list\n");
}

static void test_iGetIntList_mixed() {
    int len = 0;
    int *list = iGetIntList("test", "1,5:7,10", &len);
    // 1, 5,6,7, 10 -> 5 items
    if (len != 5) {
        printf("FAIL: mixed list length mismatch: %d != 5\n", len);
        exit(1);
    }
    if (list[0] != 1 || list[1] != 5 || list[2] != 6 || list[3] != 7 || list[4] != 10) {
        printf("FAIL: mixed list content mismatch\n");
        exit(1);
    }
    free(list);
    printf("PASS: mixed list\n");
}

static void test_iGetIntList_invalid_char() {
    int len = 0;
    exit_expected = true;
    if (setjmp(jump_buffer) == 0) {
        iGetIntList("test", "1,a,3", &len);
        printf("FAIL: invalid char did not exit\n");
        exit(1);
    } else {
        printf("PASS: invalid char exited as expected\n");
    }
    exit_expected = false;
}

static void test_iGetIntList_invalid_syntax() {
    int len = 0;
    exit_expected = true;
    if (setjmp(jump_buffer) == 0) {
        iGetIntList("test", "1:2:3", &len); // double colon
        printf("FAIL: invalid syntax did not exit\n");
        exit(1);
    } else {
        printf("PASS: invalid syntax exited as expected\n");
    }
    exit_expected = false;
}

// Tests for Swap functions
static void test_lSwapLong_identity() {
    int32_t val = 0x12345678;
    int32_t res = lSwapLong(val, false);
    assert(res == val);
    printf("test_lSwapLong_identity passed\n");
}

static void test_lSwapLong_swap() {
    int32_t val = 0x12345678;
    // 0x12345678. Low 16: 0x5678. High 16: 0x1234.
    // Swapped: High=0x5678, Low=0x1234.
    // Result: 0x56781234.
    int32_t expected = 0x56781234;
    int32_t res = lSwapLong(val, true);
    assert(res == expected);
    printf("test_lSwapLong_swap passed\n");
}

static void test_lSwapLong_swap_negative() {
    int32_t val = -2; // 0xFFFFFFFE
    // Low: 0xFFFE, High: 0xFFFF
    // Swapped: High=0xFFFE, Low=0xFFFF -> 0xFFFEFFFF
    int32_t expected = (int32_t)0xFFFEFFFF;
    int32_t res = lSwapLong(val, true);
    assert(res == expected);
    printf("test_lSwapLong_swap_negative passed\n");
}

static void test_fSwapFloat_identity() {
    float val = 1.2345f;
    float res = fSwapFloat(val, false);
    assert(res == val);
    printf("test_fSwapFloat_identity passed\n");
}

static void test_fSwapFloat_swap() {
    float val = 1.0f;

    uint16_t parts[2];
    memcpy(parts, &val, 4);

    uint16_t swapped_parts[2];
    swapped_parts[0] = parts[1];
    swapped_parts[1] = parts[0];

    float expected;
    memcpy(&expected, swapped_parts, 4);

    float res = fSwapFloat(val, true);

    // Memcmp is safer for bitwise equality.
    assert(memcmp(&res, &expected, 4) == 0);
    printf("test_fSwapFloat_swap passed\n");
}

int main() {
    printf("Running tests...\n");

    // iGetIntList tests
    test_iGetIntList_simple();
    test_iGetIntList_range();
    test_iGetIntList_mixed();
    test_iGetIntList_invalid_char();
    test_iGetIntList_invalid_syntax();

    // Swap tests
    test_lSwapLong_identity();
    test_lSwapLong_swap();
    test_lSwapLong_swap_negative();
    test_fSwapFloat_identity();
    test_fSwapFloat_swap();

    printf("All tests passed.\n");
    return 0;
}
