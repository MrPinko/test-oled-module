/*
 * Copyright (c) 2024 Federico (MrPinko)
 * SPDX-License-Identifier: MIT
 *
 * OLED toggle behavior: pressing this key turns the OLED display on or off.
 *
 * Uses BEHAVIOR_LOCALITY_GLOBAL so the toggle is reflected on both halves of
 * a split keyboard (the central half sends the event to the peripheral via
 * the ZMK split subsystem).
 *
 * The Zephyr display driver API is used:
 *   - display_blanking_on(dev)  → turns the display off (power saved)
 *   - display_blanking_off(dev) → turns the display back on
 *
 * Note: some OLED hardware cannot reinitialise after full power-off.
 * display_blanking_on/off only blanks the screen (backlight/content hidden),
 * it does not cut power to the controller, so reinitialisation is not needed.
 */

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define DT_DRV_COMPAT zmk_behavior_oled_toggle

/* Module-level state: tracks whether the OLED is currently on. */
static bool oled_on = true;

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display)) {
        LOG_WRN("Display device not ready; cannot toggle OLED");
        return -ENODEV;
    }

    oled_on = !oled_on;

    int ret;
    if (oled_on) {
        ret = display_blanking_off(display);
        LOG_DBG("OLED turned ON (blanking off), ret=%d", ret);
    } else {
        ret = display_blanking_on(display);
        LOG_DBG("OLED turned OFF (blanking on), ret=%d", ret);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_oled_toggle_driver_api = {
    .binding_pressed  = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    /*
     * BEHAVIOR_LOCALITY_GLOBAL: the behavior runs on whichever half receives
     * the key-press, but ZMK forwards the event to the other half too.
     * This is the correct locality for display-control behaviors.
     */
    .locality         = BEHAVIOR_LOCALITY_GLOBAL,
};

/* Instantiate one device node for every DT node with our compatible string. */
#define OLED_TOGGLE_INST(n)                                                 \
    BEHAVIOR_DT_INST_DEFINE(n,                                              \
                            NULL,                   /* no init needed */    \
                            NULL,                   /* no PM device */      \
                            NULL,                   /* no instance data */  \
                            NULL,                   /* no config data */    \
                            POST_KERNEL,                                    \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,            \
                            &behavior_oled_toggle_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OLED_TOGGLE_INST)