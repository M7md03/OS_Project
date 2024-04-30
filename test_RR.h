#include <cmocka.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include "RR.h"

// Test case for createRoundRobin function
static void test_createRoundRobin(void **state) {
    struct RoundRobin *rr = createRoundRobin(5);
    assert_non_null(rr);
    assert_int_equal(rr->capacity, 1);
    assert_int_equal(rr->front, -1);
    assert_int_equal(rr->rear, -1);
    assert_int_equal(rr->quantum, 5);
    assert_int_equal(rr->runQuantum, 5);
    FreeRoundRobin(rr);
}

// Test case for isEmpty function
static void test_isEmpty(void **state) {
    struct RoundRobin *rr = createRoundRobin(5);
    assert_true(isEmpty(rr));
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    assert_true(enqueue(rr, p));
    assert_false(isEmpty(rr));
    FreeRoundRobin(rr);
    free(p);
}

// Test case for enqueue and dequeue functions
static void test_enqueue_dequeue(void **state) {
    struct RoundRobin *rr = createRoundRobin(5);
    struct Process *p1 = (struct Process *)malloc(sizeof(struct Process));
    struct Process *p2 = (struct Process *)malloc(sizeof(struct Process));
    p1->ID = 1;
    p2->ID = 2;
    assert_true(enqueue(rr, p1));
    assert_true(enqueue(rr, p2));
    assert_int_equal(rr->front, 0);
    assert_int_equal(rr->rear, 1);
    struct Process *deq_p1 = dequeue(rr);
    assert_int_equal(deq_p1->ID, 1);
    assert_int_equal(rr->front, 1);
    assert_int_equal(rr->rear, 1);
    struct Process *deq_p2 = dequeue(rr);
    assert_int_equal(deq_p2->ID, 2);
    assert_true(isEmpty(rr));
    FreeRoundRobin(rr);
    free(p1);
    free(p2);
}

// Test case for FreeRoundRobin function
static void test_FreeRoundRobin(void **state) {
    struct RoundRobin *rr = createRoundRobin(5);
    struct Process *p = (struct Process *)malloc(sizeof(struct Process));
    assert_true(enqueue(rr, p));
    FreeRoundRobin(rr);
    // Check if memory is freed
    assert_null(rr);
    assert_null(p);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_createRoundRobin),
        cmocka_unit_test(test_isEmpty),
        cmocka_unit_test(test_enqueue_dequeue),
        cmocka_unit_test(test_FreeRoundRobin),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}