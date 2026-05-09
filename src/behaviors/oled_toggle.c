#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>

// Only include split headers on central side
#if IS_ENABLED(CONFIG_ZMK_SPLIT) && IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
#include <zmk/split/bluetooth/central.h>
#define IS_CENTRAL 1
#else
#define IS_CENTRAL 0
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define DT_DRV_COMPAT zmk_behavior_oled_toggle

static bool oled_on = true;

static void toggle_local_display(void) {
    const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display)) {
        LOG_WRN("Display not ready, skipping toggle");
        return;
    }

    oled_on = !oled_on;

    if (oled_on) {
        display_blanking_off(display);
    } else {
        display_blanking_on(display);
    }
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    // Always toggle local side (works for both central and peripheral)
    toggle_local_display();

#if IS_CENTRAL
    // Central: also invoke the same behavior on the peripheral
    // peripheral index 0 = first peripheral (right half)
    zmk_split_bt_invoke_behavior(0, binding, event, true);
#endif

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
#if IS_CENTRAL
    zmk_split_bt_invoke_behavior(0, binding, event, false);
#endif
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_oled_toggle_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

#define OLED_TOGGLE_INST(n)                                      \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL,          \
                            POST_KERNEL,                         \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, \
                            &behavior_oled_toggle_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OLED_TOGGLE_INST)