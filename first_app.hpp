#pragma once

#include "vr_window.hpp"

namespace vr
{
    class FirstApp{

        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();
        
        private:
            VrWindow vrWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    };
    

} // namespace vr
