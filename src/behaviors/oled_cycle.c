/*
 * Copyright (c) 2024 Federico (MrPinko)
 * SPDX-License-Identifier: MIT
 *
 * OLED image / GIF cycle behavior: pressing this key/combo cycles to the
 * next image or animated GIF on the OLED display.
 *
 * Uses BEHAVIOR_LOCALITY_GLOBAL so the event is raised on both halves of a
 * split keyboard, allowing either or both screens to cycle their artwork.
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <events/oled_cycle_event.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define DT_DRV_COMPAT zmk_behavior_oled_gif_cycle

ZMK_EVENT_IMPL(zmk_oled_cycle_event);

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    LOG_DBG("OLED cycle key pressed %d -> raising zmk_oled_cycle_event", event.position);
    LOG_DBG("OLED cycle key param1 (%d)", binding->param1);
    raise_zmk_oled_cycle_event((struct zmk_oled_cycle_event){});
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_oled_gif_cycle_driver_api = {
    .binding_pressed  = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
    .locality         = BEHAVIOR_LOCALITY_GLOBAL,
};

#define OLED_CYCLE_INST(n)                                                  \
    BEHAVIOR_DT_INST_DEFINE(n,                                              \
                            NULL,                                           \
                            NULL,                                           \
                            NULL,                                           \
                            NULL,                                           \
                            POST_KERNEL,                                    \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,            \
                            &behavior_oled_gif_cycle_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OLED_CYCLE_INST)
