# Copyright (c) 2024 Federico (MrPinko)
# SPDX-License-Identifier: MIT

if(CONFIG_ZMK_BEHAVIOR_OLED_TOGGLE OR CONFIG_ZMK_BEHAVIOR_OLED_GIF_CYCLE)
  zephyr_include_directories(include)

  if(CONFIG_ZMK_BEHAVIOR_OLED_TOGGLE)
    target_sources(app PRIVATE src/behaviors/oled_toggle.c)
  endif()

  if(CONFIG_ZMK_BEHAVIOR_OLED_GIF_CYCLE)
    target_sources(app PRIVATE src/behaviors/oled_cycle.c)
  endif()
endif()