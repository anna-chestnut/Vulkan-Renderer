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

# Automatically copy shaders folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})