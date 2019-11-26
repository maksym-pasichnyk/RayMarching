cmake_minimum_required(VERSION 3.15)
project(RM)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(rosy)
include_directories(rosy/modules)
include_directories(rosy/components)


#add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
#add_compile_definitions(GLM_FORCE_LEFT_HANDED)

function(add_shaders out_var)
    set(result)
    foreach(in_f ${ARGN})
        get_filename_component(f_dir "${in_f}" DIRECTORY)
        file(RELATIVE_PATH src_f ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${in_f})
        set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}.dir")
        set(out_f "${out_dir}/${in_f}.o")
        file(RELATIVE_PATH rel_f ${CMAKE_CURRENT_BINARY_DIR} ${out_f})
        add_custom_command(
            OUTPUT ${out_f}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${out_dir}/${f_dir}" && ld -r -b binary -o ${out_f} ${src_f}
            DEPENDS ${in_f}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Building GLSL object ${rel_f}"
            VERBATIM
        )
        list(APPEND result ${out_f})
    endforeach()
    SET(${out_var} "${result}" PARENT_SCOPE)
endfunction()

add_shaders(SHADERS
    main.glsl
        rosy/core.glsl
)

add_executable(RM
        ${SHADERS}
        rosy/module.h
        rosy/modules/event/event.h
        rosy/modules/input/input.h
        rosy/modules/graphics/graphics.cpp
        rosy/modules/graphics/graphics.h
        rosy/modules/graphics/camera.cpp
        rosy/modules/graphics/camera.h
        rosy/modules/window/window.cpp
        rosy/modules/window/window.h
        rosy/modules/timer/timer.cpp
        rosy/modules/timer/timer.h
        rosy/components/transform.h
        rosy/boot.cpp main.cpp rosy/rosy.h)
target_link_libraries(RM SDL2 GL GLEW)