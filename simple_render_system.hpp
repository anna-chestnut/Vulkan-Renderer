#pragma once

#include "vr_camera.hpp"
#include "vr_pipeline.hpp"
#include "vr_device.hpp"
#include "vr_game_object.hpp"

//std
#include <memory>
#include <vector>

namespace vr
{
    class SimpleRenderSystem{

        public:

        SimpleRenderSystem(VrDevice &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VrGameObject> &gameObjects, const VrCamera &camera);
    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        VrDevice &vrDevice;
        
        std::unique_ptr<VrPipeline> vrPipeline;
        VkPipelineLayout pipelineLayout;
    };

} // namespace vr
