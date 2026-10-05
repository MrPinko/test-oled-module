# Copyright (c) 2024 Federico (MrPinko)
# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_BEHAVIOR_OLED_TOGGLE)
  target_sources(app PRIVATE src/behaviors/oled_toggle.c)
endif()