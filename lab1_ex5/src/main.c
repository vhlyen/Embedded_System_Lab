#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec y0 = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
static const struct gpio_dt_spec y1 = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);

struct phase {
    const char *name;
    int y0_on;
    int y1_on;
    int duration_ms;
};

static const struct phase phases[] = {
    { "RED",   1, 0, 7000 },
    { "GREEN", 0, 1, 5000 },
    { "AMBER", 1, 1, 2000 },
};

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
        k_msleep(1000);
        gpio_pin_set_dt(&led3, 0);
        k_msleep(1000);
    }
}

static void thread_traffic_light(void *p1, void *p2, void *p3)
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
        for (size_t i = 0; i < ARRAY_SIZE(phases); i++) {
            const struct phase *phase = &phases[i];

            gpio_pin_set_dt(&y0, phase->y0_on);
            gpio_pin_set_dt(&y1, phase->y1_on);
            printk("[LIGHT] %s - %d s\n", phase->name, phase->duration_ms / 1000);
            k_msleep(phase->duration_ms);
        }
    }
}

K_THREAD_DEFINE(thread_led_blinky_id, 1024, thread_led_blinky, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(thread_traffic_light_id, 1024, thread_traffic_light,
                NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}
