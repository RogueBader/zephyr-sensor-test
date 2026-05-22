#include <zephyr/ztest.h>
#include "ringbuf.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

ZTEST(driver_api, sensor_basic_functionality)
{
    const struct device *dev = device_get_binding("FAKE_XYZ");
    zassert_not_null(dev, "Failed to get device");

    struct sensor_value xyz[3];

    for (int i = 0; i < 20; i++) {

        zassert_equal(sensor_sample_fetch(dev), 0, "Sample fetch failed");

        zassert_equal(
            sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, xyz),
            0,
            "Channel get failed"
        );

        /* Check values are finite */
        for (int j = 0; j < 3; j++) {
            zassert_false((xyz[j].val1 == 0 && xyz[j].val2 == 0),
                          "Invalid sensor value");
        }
    }
}

ZTEST_SUITE(driver_api, NULL, NULL, NULL, NULL, NULL);

ZTEST(ringbuf, push_pop_basic)
{
    ringbuf_t *rb = rb_create(4);
    zassert_not_null(rb, "Failed to create ring buffer");

    struct accel_sample in = { .t_ms = 1, .ax_ms2 = 1.0, .ay_ms2 = 2.0, .az_ms2 = 3.0 };
    struct accel_sample out;

    zassert_true(rb_push(rb, &in), "Push failed");
    zassert_true(rb_pop(rb, &out), "Pop failed");

    zassert_equal(out.t_ms, in.t_ms, "Timestamp mismatch");
    zassert_equal(out.ax_ms2, in.ax_ms2, "X mismatch");
    zassert_equal(out.ay_ms2, in.ay_ms2, "Y mismatch");
    zassert_equal(out.az_ms2, in.az_ms2, "Z mismatch");

    rb_destroy(rb);
}

ZTEST(ringbuf, wrap_behavior)
{
    ringbuf_t *rb = rb_create(4);

    struct accel_sample s = {0};

    /* Fill buffer */
    for (int i = 0; i < 3; i++) {
        zassert_true(rb_push(rb, &s), "Push failed");
    }

    /* Pop one */
    struct accel_sample out;
    zassert_true(rb_pop(rb, &out), "Pop failed");

    /* Push again (should wrap) */
    zassert_true(rb_push(rb, &s), "Wrap push failed");

    zassert_equal(rb_size(rb), 3, "Wrap size incorrect");

    rb_destroy(rb);
}

ZTEST(ringbuf, full_and_drop)
{
    ringbuf_t *rb = rb_create(4);
    struct accel_sample s = {0};

    /* Fill to capacity (capacity - 1 usable) */
    for (int i = 0; i < 3; i++) {
        zassert_true(rb_push(rb, &s), "Push failed");
    }

    /* Next push should fail */
    zassert_false(rb_push(rb, &s), "Push should fail when full");

    rb_destroy(rb);
}

ZTEST(ringbuf, bulk_drain)
{
    ringbuf_t *rb = rb_create(8);

    struct accel_sample s = { .t_ms = 1 };
    struct accel_sample out[5];

    for (int i = 0; i < 5; i++) {
        s.t_ms = i;
        rb_push(rb, &s);
    }

    size_t count = rb_drain(rb, out, 5);

    zassert_equal(count, 5, "Drain count mismatch");

    for (int i = 0; i < 5; i++) {
        zassert_equal(out[i].t_ms, i, "Order mismatch");
    }

    rb_destroy(rb);
}

ZTEST_SUITE(ringbuf, NULL, NULL, NULL, NULL, NULL);