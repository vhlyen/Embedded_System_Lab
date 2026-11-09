#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

struct blink_cfg {
    struct gpio_dt_spec led;
    int on_ms;
    int off_ms;
};

static struct blink_cfg led3_cfg = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    .on_ms = 2000,
    .off_ms = 2000,
};

static struct blink_cfg y0_cfg = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    .on_ms = 500,
    .off_ms = 500,
};

static struct blink_cfg y1_cfg = {
    .led = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    .on_ms = 3000,
    .off_ms = 1000,
};

static void thread_blink(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    const struct blink_cfg *cfg = p1;

    if (!gpio_is_ready_dt(&cfg->led) ||
        gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE) < 0) {
        return;
    }

    while (1) {
        gpio_pin_set_dt(&cfg->led, 1);
        k_msleep(cfg->on_ms);
        gpio_pin_set_dt(&cfg->led, 0);
        k_msleep(cfg->off_ms);
    }
}

/* One function can serve all three lights because p1 selects each thread's
 * GPIO and timing configuration. Separate functions would repeat identical
 * logic and differ only in those values. */
K_THREAD_DEFINE(thread_blink_led3_id, 1024, thread_blink,
                &led3_cfg, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(thread_blink_y0_id, 1024, thread_blink,
                &y0_cfg, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(thread_blink_y1_id, 1024, thread_blink,
                &y1_cfg, NULL, NULL, 7, 0, 0);

int main(void)
{
    return 0;
}
