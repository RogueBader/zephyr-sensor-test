#include <zephyr/ztest.h>
#include "ringbuf.h"

/* Test: simple push and drain */
ZTEST(ringbuf, push_pop_basic)
{
    ringbuf_t *rb = rb_create(4);
    zassert_not_null(rb, "Failed to create ring buffer");

    struct accel_sample s1 = { .t_ms = 1, .ax_ms2 = 1.0f, .ay_ms2 = 2.0f, .az_ms2 = 3.0f };
    struct accel_sample out[1];

    /* Push */
    bool ok = rb_push(rb, &s1);
    zassert_true(ok, "Push failed");

    /* Drain */
    size_t n = rb_drain(rb, out, 1);
    zassert_equal(n, 1, "Drain count incorrect");

    /* Check values */
    zassert_equal(out[0].t_ms, 1, "Timestamp mismatch");
    zassert_within(out[0].ax_ms2, 1.0f, 0.001f, "ax mismatch");
    zassert_within(out[0].ay_ms2, 2.0f, 0.001f, "ay mismatch");
    zassert_within(out[0].az_ms2, 3.0f, 0.001f, "az mismatch");
}

ZTEST_SUITE(ringbuf, NULL, NULL, NULL, NULL, NULL);

void test_main(void)
{
    ztest_run_all(NULL, false, 1, 1);
}
