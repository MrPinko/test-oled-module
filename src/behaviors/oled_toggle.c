#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/dlist.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/hid.h>
#include <zmk/matrix.h>
#include <zmk/keymap.h>

#define DT_DRV_COMPAT zmk_behavior_oled_toggle

static bool oled_on = true;

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display)) {
        return -ENODEV;
    }

    oled_on = !oled_on;

    if (oled_on) {
        display_blanking_off(display);
    } else {
        display_blanking_on(display);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_oled_toggle_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality = BEHAVIOR_LOCALITY_GLOBAL,    // ← this is all you need
};


#define OLED_TOGGLE_INST(n)                                          \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL,              \
                            POST_KERNEL,                             \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,     \
                            &behavior_oled_toggle_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OLED_TOGGLE_INST)