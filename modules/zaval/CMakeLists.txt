merc_add_faust(zaval-faust ${CMAKE_CURRENT_SOURCE_DIR}/zaval.dsp)
merc_add_module(zaval zaval.cpp
    GUID 2fba777c-5a8c-4a9a-b293-c3c8d85da655
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(zaval PRIVATE zaval-faust)
add_dependencies(zaval moduleinfotool)
