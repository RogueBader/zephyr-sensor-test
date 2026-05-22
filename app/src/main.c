#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "ringbuf.h"

LOG_MODULE_REGISTER(app);

#define SAMPLE_BATCH 20

static ringbuf_t *rb;

/* ---------------- PRODUCER (sensor → ring buffer) ---------------- */

static void producer_work_handler(struct k_work *work);
K_WORK_DEFINE(producer_work, producer_work_handler);

static struct k_timer producer_timer;

static void producer_timer_handler(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    k_work_submit(&producer_work);
}

static void producer_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    const struct device *dev = device_get_binding("FAKE_XYZ");
    if (!dev) {
        LOG_ERR("Sensor device not found");
        return;
    }

    struct sensor_value xyz[3];
    struct accel_sample sample;

    if (sensor_sample_fetch(dev) != 0) {
        LOG_ERR("Sample fetch failed");
        return;
    }

    if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, xyz) != 0) {
        LOG_ERR("Channel get failed");
        return;
    }

    sample.t_ms = k_uptime_get();
    sample.ax_ms2 = xyz[0].val1 + xyz[0].val2 / 1e6;
    sample.ay_ms2 = xyz[1].val1 + xyz[1].val2 / 1e6;
    sample.az_ms2 = xyz[2].val1 + xyz[2].val2 / 1e6;

    if (!rb_push(rb, &sample)) {
        LOG_WRN("Ring buffer full, sample dropped");
    }
}

/* ---------------- CONSUMER (ring buffer → UART/log) ---------------- */

void consumer_thread(void)
{
    struct accel_sample buffer[SAMPLE_BATCH];

    while (1) {
        size_t count = rb_drain(rb, buffer, SAMPLE_BATCH);

        for (size_t i = 0; i < count; i++) {
            LOG_INF("{\"t\":%lld,\"x\":%.3f,\"y\":%.3f,\"z\":%.3f}",
                    buffer[i].t_ms,
                    buffer[i].ax_ms2,
                    buffer[i].ay_ms2,
                    buffer[i].az_ms2);
        }

        k_msleep(50); // 20 Hz consumer
    }
}

/* ---------------- MAIN ---------------- */

int main(void)
{
    LOG_INF("Starting sensor app...");

    rb = rb_create(RB_CAPACITY);
    if (!rb) {
        LOG_ERR("Failed to create ring buffer");
        return -1;
    }

    int period_ms = 1000 / 100; // 100 Hz producer

    k_timer_init(&producer_timer, producer_timer_handler, NULL);
    k_timer_start(&producer_timer, K_MSEC(period_ms), K_MSEC(period_ms));

    consumer_thread();

    return 0;
}