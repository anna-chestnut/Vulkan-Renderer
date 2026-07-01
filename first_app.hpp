#pragma once

#include "vr_window.hpp"
#include "vr_pipeline.hpp"
#include "vr_device.hpp"
#include "vr_swap_chain.hpp"

//std
#include <memory>
#include <vector>

namespace vr
{
    class FirstApp{

        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        VrWindow vrWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VrDevice vrDevice{vrWindow};
        VrSwapChain vrSwapChain{vrDevice, vrWindow.getExtent()};
        std::unique_ptr<VrPipeline> vrPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
    };

} // namespace vr
