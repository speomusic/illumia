merc_add_faust(buzz-faust ${CMAKE_CURRENT_SOURCE_DIR}/buzz.dsp)
merc_add_module(buzz buzz.cpp
    GUID 63bf7d56-deb7-458e-8f8e-407e0390de34
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(buzz PRIVATE buzz-faust)
add_dependencies(buzz moduleinfotool)
