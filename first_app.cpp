#include "first_app.hpp"

namespace vr{
    void FirstApp::run(){
        while(!vrWindow.shouldClose()){
            glfwPollEvents();
        }
    }
}