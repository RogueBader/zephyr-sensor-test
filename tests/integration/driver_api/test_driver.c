#include <zephyr/ztest.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

ZTEST(driver_api, sensor_basic_functionality)
{
    const struct device *dev = device_get_binding("FAKE_XYZ");
    zassert_not_null(dev, "Device not found");

    struct sensor_value xyz[3];
    int64_t last_time = -1;

    for (int i = 0; i < 20; i++) {

        /* Fetch new sample */
        int ret = sensor_sample_fetch(dev);
        zassert_equal(ret, 0, "sample_fetch failed");

        /* Get accelerometer data */
        ret = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, xyz);
        zassert_equal(ret, 0, "channel_get failed");

        /* ✅ Check values are NOT zero */
        for (int j = 0; j < 3; j++) {
            zassert_false(
                (xyz[j].val1 == 0 && xyz[j].val2 == 0),
                "Invalid sensor data"
            );
        }

        /* ✅ Check timestamp is monotonic (increasing) */
        int64_t current_time = k_uptime_get();
        zassert_true(current_time >= last_time, "Timestamp not monotonic");
        last_time = current_time;
    }
}

ZTEST_SUITE(driver_api, NULL, NULL, NULL, NULL, NULL);