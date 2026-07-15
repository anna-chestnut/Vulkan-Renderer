cmake_minimum_required(VERSION 3.15)
project(Vulkan-Renderer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(Vulkan-Renderer 
    main.cpp 
    first_app.cpp 
    vr_window.cpp
    vr_pipeline.cpp
    vr_device.cpp
    vr_swap_chain.cpp
    vr_model.cpp
    vr_renderer.cpp
    simple_render_system.cpp
    vr_camera.cpp
    keyboard_movement_controller.cpp
    vr_game_object.cpp
    vr_buffer.cpp
)

target_include_directories(Vulkan-Renderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/tinyobjectloader
)

target_link_libraries(Vulkan-Renderer PRIVATE 
    Vulkan::Vulkan 
    glfw
    glm::glm
)

# This is the "Mac Special" that makes his logic work on your Homebrew setup:
if(APPLE)
    target_link_libraries(Vulkan-Renderer PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework QuartzCore"
    )
endif()

# Compile GLSL shaders to SPIR-V
find_program(GLSLC glslc
    HINTS
        $ENV{VULKAN_SDK}/bin
        /Users/annachen/VulkanSDK/1.4.350.1/macOS/bin
)

if(NOT GLSLC)
    message(FATAL_ERROR "glslc not found. Make sure Vulkan SDK is installed and VULKAN_SDK is set.")
endif()

file(GLOB_RECURSE SHADER_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
)

set(SPIRV_BINARY_FILES "")

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)

    set(SPIRV_OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv"
    )

    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${GLSLC} ${SHADER} -o ${SPIRV_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${SHADER_NAME}"
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUTPUT})
endforeach()

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Vulkan-Renderer Shaders)

# Copy model assets into the build directory.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/models
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})