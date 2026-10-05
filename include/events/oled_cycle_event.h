/*
 * Copyright (c) 2024 Federico (MrPinko)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_oled_cycle_event {
    uint8_t dummy;
};

ZMK_EVENT_DECLARE(struct zmk_oled_cycle_event);
