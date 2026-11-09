#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

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
        k_msleep(1500);
        gpio_pin_set_dt(&led3, 0);
        k_msleep(500);
    }
}

static void thread_output_sequence(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    if (!gpio_is_ready_dt(&y0) || !gpio_is_ready_dt(&y1) ||
        gpio_pin_configure_dt(&y0, GPIO_OUTPUT_INACTIVE) < 0 ||
        gpio_pin_configure_dt(&y1, GPIO_OUTPUT_INACTIVE) < 0) {
        return;
    }

    while (1) {
        /* Only Y0 on for 1 s. */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 0);
        k_msleep(1000);

        /* Only Y1 on for 2 s. */
        gpio_pin_set_dt(&y0, 0);
        gpio_pin_set_dt(&y1, 1);
        k_msleep(2000);

        /* Both on for 2 s. */
        gpio_pin_set_dt(&y0, 1);
        gpio_pin_set_dt(&y1, 1);
        k_msleep(2000);
    }
}

K_THREAD_DEFINE(thread_led_blinky_id, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(thread_output_sequence_id, 1024, thread_output_sequence,
                NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}
