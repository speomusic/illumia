merc_add_faust(trancegate-faust ${CMAKE_CURRENT_SOURCE_DIR}/trancegate.dsp)
merc_add_module(trancegate trancegate.cpp
    GUID 4321bd53-ca7a-49e9-b669-f788c560c391
    VENDOR Speo
    URL https://speomusic.com
    EMAIL jan@speomusic.com)
target_link_libraries(trancegate PRIVATE trancegate-faust)
add_dependencies(trancegate moduleinfotool)
