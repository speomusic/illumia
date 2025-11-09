merc_add_faust(goodwin-faust ${CMAKE_CURRENT_SOURCE_DIR}/goodwin.dsp)
merc_add_module(goodwin goodwin.cpp
    GUID cdee539d-57ca-43c0-84c9-6cc16086c1e4
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(goodwin PRIVATE goodwin-faust)
add_dependencies(goodwin moduleinfotool)
