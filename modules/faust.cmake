function(illumia_add_faust target_name dsp_file_path)
    cmake_path(GET dsp_file_path FILENAME dsp_file)
    cmake_path(GET dsp_file_path STEM dsp_stem)
    file(WRITE ${CMAKE_BINARY_DIR}/include/illumia/${target_name}.h
        "#include \"illumia/faust.h\"\n"
        "#include \"${dsp_file}.h\"\n"
        "namespace illumia::faust { using ${dsp_stem} = FaustWrapper<generated::${dsp_stem}>; }\n")
    set(out_file_path ${CMAKE_BINARY_DIR}/include/illumia/${dsp_file}.h)
    find_program(FAUST_PATH faust)
    add_custom_command(
        OUTPUT ${out_file_path}
        COMMAND ${FAUST_PATH} ${dsp_file_path} -nvi -cn ${dsp_stem} -ns illumia::faust::generated -o ${out_file_path}
        DEPENDS ${dsp_file_path}
        VERBATIM)
    add_library(${target_name} INTERFACE)
    target_sources(${target_name} PRIVATE ${dsp_file_path} ${out_file_path})
    target_include_directories(${target_name} INTERFACE ${CMAKE_BINARY_DIR}/include)
endfunction()
