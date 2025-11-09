merc_add_faust(deltick-faust ${CMAKE_CURRENT_SOURCE_DIR}/deltick.dsp)
merc_add_module(deltick deltick.cpp
    GUID 984417db-b8f4-437a-8de2-d893b6355da5
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(deltick PRIVATE deltick-faust)
add_dependencies(deltick moduleinfotool)
