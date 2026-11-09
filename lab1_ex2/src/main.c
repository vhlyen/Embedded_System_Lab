#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/atomic.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

/* The writer publishes LED3's logical state; the follower only reads it. */
static atomic_t led3_is_on;

static void thread_led_blinky(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    if (!gpio_is_ready_dt(&led3) || gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE) < 0) {
        return;
    }

    while (1) {
        gpio_pin_set_dt(&led3, 1);
        atomic_set(&led3_is_on, 1);
        k_msleep(2000);

        gpio_pin_set_dt(&led3, 0);
        atomic_set(&led3_is_on, 0);
        k_msleep(2000);
    }
}

static void thread_y0_follow(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    if (!gpio_is_ready_dt(&y0) || gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE) < 0) {
        return;
    }

    while (1) {
        gpio_pin_set_dt(&y0, atomic_get(&led3_is_on) ? 1 : 0);
        k_msleep(10);
    }
}

K_THREAD_DEFINE(thread_led_blinky_id, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(thread_y0_follow_id, 1024, thread_y0_follow, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}
