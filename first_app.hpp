#pragma once

#include "vr_window.hpp"
#include "vr_pipeline.hpp"
#include "vr_device.hpp"

namespace vr
{
    class FirstApp{

        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

        private:
            VrWindow vrWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            VrDevice vrDevice{vrWindow};
            VrPipeline vrPipeline{
                vrDevice,
                "shaders/simple_shader.vert.spv",
                "shaders/simple_shader.frag.spv",
                VrPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
    

} // namespace vr
