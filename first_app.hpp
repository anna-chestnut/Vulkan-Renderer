#pragma once

#include "vr_window.hpp"
#include "vr_pipeline.hpp"

namespace vr
{
    class FirstApp{

        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

        private:
            VrWindow vrWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            VrPipeline vrPipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv"};
    };
    

} // namespace vr
