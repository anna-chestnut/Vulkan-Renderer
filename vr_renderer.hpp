#pragma once

#include "vr_window.hpp"
#include "vr_device.hpp"
#include "vr_swap_chain.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace vr
{
    class VrRenderer{

        public:

        VrRenderer(VrWindow &vrWindow, VrDevice &vrDevice);
        ~VrRenderer();

        VrRenderer(const VrRenderer &) = delete;
        VrRenderer &operator=(const VrRenderer &) = delete;

        float getAspectRatio() const { return vrSwapChain->extentAspectRatio(); }
        bool isFrameInProgress() const {return isFrameStarted;}

        VkCommandBuffer getCurrentCommandBuffer() const { 
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex]; 
        }

        int getFrameIndex() const{
            assert(isFrameStarted && "Cannot get frame index when frame not in progess");
            return currentFrameIndex;
        }

        VkFormat getSwapChainImageFormat() const
        {
            return vrSwapChain->getSwapChainImageFormat();
        }

        VkFormat getSwapChainDepthFormat() const
        {
            return vrSwapChain->getDepthFormat();
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRendering(VkCommandBuffer commandBuffer);
        void endSwapChainRendering(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        VrWindow &vrWindow;
        VrDevice &vrDevice;
        std::unique_ptr<VrSwapChain> vrSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex{0};
        int currentFrameIndex{0};
        bool isFrameStarted{false};

        void transitionImageLayout(
            VkCommandBuffer commandBuffer,
            VkImage image,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkPipelineStageFlags2 srcStageMask,
            VkAccessFlags2 srcAccessMask,
            VkPipelineStageFlags2 dstStageMask,
            VkAccessFlags2 dstAccessMask,
            VkImageAspectFlags aspectMask);
    };

} // namespace vr
