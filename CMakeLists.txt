cmake_minimum_required(VERSION 3.15)
project(Vulkan-Renderer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(Vulkan-Renderer 
    main.cpp 
    first_app.cpp 
    vr_window.cpp
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