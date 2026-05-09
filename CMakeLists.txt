if (CONFIG_ZMK_DISPLAY)
  target_sources(app PRIVATE src/behaviors/oled_toggle.c)

  zephyr_include_directories(include)
endif()