function(merc_add_module target_name)
    set(infos VENDOR URL EMAIL)
    set(unaries GUID GENERATED_GUID_COUNT ${infos})
    set(multies SHADERS RESOURCES)
    cmake_parse_arguments(MERC_MODULE "" "${unaries}" "${multies}" ${ARGN})
    if(NOT DEFINED MERC_MODULE_GENERATED_GUID_COUNT)
        set(MERC_MODULE_GENERATED_GUID_COUNT 10)
    endif()
    set(SOURCES ${MERC_MODULE_UNPARSED_ARGUMENTS})

    add_library(${target_name} MODULE ${SOURCES} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/module.cpp)
    target_link_libraries(${target_name} PRIVATE merc)
    foreach(info ${infos})
        target_compile_definitions(${target_name}
            PRIVATE "MERC_MODULE_${info}=\"${MERC_MODULE_${info}}\"")
    endforeach()

    foreach(i RANGE ${MERC_MODULE_GENERATED_GUID_COUNT})
        string(UUID guid NAMESPACE ${MERC_MODULE_GUID} NAME ${i} TYPE MD5)
        string(REPLACE - "" tuid ${guid})
        list(APPEND guids \"${tuid}\")
    endforeach()
    list(JOIN guids , MERC_GENERATED_GUIDS)
    target_compile_definitions(${target_name} PRIVATE
        "MERC_GENERATED_GUIDS=${MERC_GENERATED_GUIDS}"
        "MERC_GENERATED_GUID_COUNT=${MERC_MODULE_GENERATED_GUID_COUNT}")

    set(vst3_dir ${CMAKE_BINARY_DIR}/vst3)
    if(WIN32)
        set_target_properties(${target_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY $<1:${vst3_dir}/${target_name}.vst3/Contents/x86_64-win>
            SUFFIX .vst3)
        set(MERC_MODULE_RESOURCES ${MERC_MODULE_RESOURCES} ${MERC_MODULE_SHADERS})
    elseif(APPLE)
        set_target_properties(${target_name} PROPERTIES
            BUNDLE true
            BUNDLE_EXTENSION vst3
            LIBRARY_OUTPUT_DIRECTORY $<1:${vst3_dir}>)
        target_compile_options(${target_name} PRIVATE "-fvisibility=hidden")
        target_compile_definitions(${target_name} PRIVATE $<$<CONFIG:Debug>:DEVELOPMENT>)
        target_sources(${target_name} PRIVATE ${MERC_MODULE_SHADERS})
        set_source_files_properties(${MERC_MODULE_SHADERS} PROPERTIES LANGUAGE METAL)
    endif()

    set(VST_MODULEINFOTOOL ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/moduleinfotool)
    set(module_path ${vst3_dir}/${target_name}.vst3)
    set(moduleinfo_path ${module_path}/Contents/Resources/moduleinfo.json)
    set(moduleinfo_commands
        COMMAND ${CMAKE_COMMAND} -E make_directory ${module_path}/Contents/Resources
        COMMAND ${VST_MODULEINFOTOOL} -create -version 1.0.0 -path ${module_path} -output ${moduleinfo_path})
    if(APPLE)
        set(moduleinfo_commands COMMAND codesign -s - -f ${module_path} ${moduleinfo_commands})
    endif() 
    add_custom_command(TARGET ${target_name}
                       POST_BUILD
                       ${moduleinfo_commands})

    foreach(resource ${MERC_MODULE_RESOURCES})
        cmake_path(GET resource FILENAME resource_filename)
        configure_file(${resource} ${module_path}/Contents/Resources/${resource_filename} COPYONLY)
        target_sources(${target_name} PRIVATE ${resource})
        # prevent offline shader compilation
        set_source_files_properties(${resource} PROPERTIES HEADER_FILE_ONLY TRUE)
    endforeach()
endfunction()
