#pragma once

#include "vr_camera.hpp"
#include "vr_pipeline.hpp"
#include "vr_device.hpp"
#include "vr_frame_info.hpp"
#include "vr_game_object.hpp"

//std
#include <memory>
#include <vector>

namespace vr
{
    class SimpleRenderSystem{

        public:
            SimpleRenderSystem(
                VrDevice &device,
                VkFormat colorAttachmentFormat,
                VkFormat depthAttachmentFormat);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

            void renderGameObjects(FrameInfo &frameInfo, std::vector<VrGameObject> &gameObjects);

        private:
            void createPipelineLayout();
            void createPipeline(VkFormat colorAttachmentFormat, VkFormat depthAttachmentFormat);

            VrDevice &vrDevice;

            std::unique_ptr<VrPipeline> vrPipeline;
            VkPipelineLayout pipelineLayout;
    };

} // namespace vr
