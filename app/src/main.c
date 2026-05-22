#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "ringbuf.h"

LOG_MODULE_REGISTER(app);

#define SAMPLE_BATCH 20

static ringbuf_t *rb;

/* ---------- PRODUCER ---------- */

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

    struct accel_sample sample;

    int64_t t = k_uptime_get();

    sample.t_ms = t;
    sample.ax_ms2 = (float)(t % 100) / 50.0f;
    sample.ay_ms2 = (float)(t % 50) / 50.0f;
    sample.az_ms2 = 9.80f;

    if (rb_push(rb, &sample) != 0) {
        LOG_WRN("Ring buffer full");
    }
}

/* ---------- CONSUMER ---------- */

void consumer_thread(void)
{
    struct accel_sample buffer[SAMPLE_BATCH];

    while (1) {
        int count = rb_drain(rb, buffer, SAMPLE_BATCH);

        for (int i = 0; i < count; i++) {
            LOG_INF("{\"t\":%lld,\"x\":%.3f,\"y\":%.3f,\"z\":%.3f}",
                    buffer[i].t_ms,
                    (double)buffer[i].ax_ms2,
                    (double)buffer[i].ay_ms2,
                    (double)buffer[i].az_ms2);
        }

        k_sleep(K_MSEC(100));
    }
}

/* ---------- MAIN ---------- */

K_THREAD_DEFINE(consumer_tid, 1024, consumer_thread, NULL, NULL, NULL,
                5, 0, 0);

int main(void)
{
    LOG_INF("Starting simulated sensor app");

    rb = rb_create(64);
    if (!rb) {
        LOG_ERR("Ring buffer create failed");
        return 0;
    }

    k_timer_init(&producer_timer, producer_timer_handler, NULL);
    k_timer_start(&producer_timer, K_NO_WAIT, K_MSEC(100));

    return 0;
}
