#pragma once

#include "vr_window.hpp"
#include "vr_pipeline.hpp"
#include "vr_device.hpp"
#include "vr_swap_chain.hpp"
#include "vr_model.hpp"

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
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);

        VrWindow vrWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VrDevice vrDevice{vrWindow};
        std::unique_ptr<VrSwapChain> vrSwapChain;
        std::unique_ptr<VrPipeline> vrPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<VrModel> vrModel;
    };

} // namespace vr
