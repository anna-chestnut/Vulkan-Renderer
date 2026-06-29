#include "vr_window.hpp"

namespace vr
{
    VrWindow::VrWindow(int w, int h, std::string name) : width(w), height(h), windowName(name) {
        initWindow();
    }

    VrWindow::~VrWindow(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VrWindow::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }
    
    void VrWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("failed to create window surface");
        }

    }

} // namespace vr