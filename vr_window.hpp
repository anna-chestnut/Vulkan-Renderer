#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vr{
    class VrWindow{
        public:
            VrWindow(int w, int h, std::string name);
            ~VrWindow();

            VrWindow(const VrWindow &) = delete;
            VrWindow &operator=(const VrWindow &) = delete;

            bool shouldClose() { return glfwWindowShouldClose(window); }
        private:
            void initWindow();

            const int width;
            const int height;

            std::string windowName;
             
            GLFWwindow *window;
    };
} // namespace vr