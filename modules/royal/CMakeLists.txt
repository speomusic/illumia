merc_add_faust(royal-faust ${CMAKE_CURRENT_SOURCE_DIR}/royal.dsp)
merc_add_module(royal royal.cpp
    GUID b7d45190-07f3-4727-a607-5210c27a968c
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(royal PRIVATE royal-faust)
add_dependencies(royal moduleinfotool)
