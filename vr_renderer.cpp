#include "vr_renderer.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vr
{

    VrRenderer::VrRenderer(VrWindow &window, VrDevice &device)
        : vrWindow{window}, vrDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    VrRenderer::~VrRenderer() { freeCommandBuffers(); }

    void VrRenderer::recreateSwapChain()
    {
        auto extent = vrWindow.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = vrWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(vrDevice.device());

        if (vrSwapChain == nullptr)
        {
            vrSwapChain = std::make_unique<VrSwapChain>(vrDevice, extent);
        }
        else
        {
            // Move the current swap chain into oldSwapChain so it stays alive while the new
            // swap chain is created. Passing it into the constructor lets Vulkan replace the
            // old swap chain safely instead of creating an unrelated one.
            std::shared_ptr<VrSwapChain> oldSwapChain = std::move(vrSwapChain);
            vrSwapChain = std::make_unique<VrSwapChain>(vrDevice, extent, oldSwapChain);

            // The existing graphics pipeline can only keep working if the new swap chain
            // uses compatible color/depth formats. If not, the pipeline must be recreated.
            if (!oldSwapChain->compareSwapFormats(*vrSwapChain.get()))
            {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
    }

    void VrRenderer::createCommandBuffers()
    {
        commandBuffers.resize(VrSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vrDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vrDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void VrRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(
            vrDevice.device(),
            vrDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer VrRenderer::beginFrame()
    {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        // Try to get the next available swap chain image to render into.
        // Vulkan writes the acquired image's index into currentImageIndex.
        // If the swap chain is out of date, usually because the window was resized,
        // recreate it and skip this frame.
        auto result = vrSwapChain->acquireNextImage(&currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }

        // VK_SUBOPTIMAL_KHR means the swap chain still works, but it may no longer
        // perfectly match the window. We can continue rendering for now.
        // Any other result means acquiring the image failed.
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        // Begin command buffer
        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        return commandBuffer;
    }

    void VrRenderer::endFrame()
    {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

        // End command buffer
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        // Submit the finished command buffer to the graphics queue and present the image.
        // If the swap chain is no longer compatible with the window, such as after resize
        // or because Vulkan reports it as out of date/suboptimal, recreate the swap chain
        // before continuing to the next frame.
        auto result = vrSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            vrWindow.wasWindowResized())
        {
            vrWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % VrSwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    void VrRenderer::beginSwapChainRendering(VkCommandBuffer commandBuffer)
    {
        assert(
            isFrameStarted &&
            "Can't call beginSwapChainRendering if frame is not in progress");
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Can't begin rendering on command buffer from a different frame");

        // Dynamic Rendering does not perform the implicit attachment transitions
        // that were previously described by VkRenderPass.
        //
        // We clear the color attachment every frame, so the previous contents are
        // not needed. Using UNDEFINED allows Vulkan to discard those contents.
        transitionImageLayout(
            commandBuffer,
            vrSwapChain->getSwapChainImage(currentImageIndex),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_2_NONE,
            VK_ACCESS_2_NONE,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT);

        // Prepare the depth image for depth testing and writing.
        // Its previous contents can also be discarded because depth is cleared
        // at the start of every frame.
        transitionImageLayout(
            commandBuffer,
            vrSwapChain->getDepthImage(currentImageIndex),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            VK_PIPELINE_STAGE_2_NONE,
            VK_ACCESS_2_NONE,
            VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            VK_IMAGE_ASPECT_DEPTH_BIT);

        // Unlike a framebuffer, Dynamic Rendering receives the actual ImageView
        // directly when the command buffer is recorded.
        //
        // imageLayout declares the layout Vulkan should expect here; it does not
        // perform the transition. The barrier above performs the real transition.
        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = vrSwapChain->getSwapChainImageView(currentImageIndex);
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.01f, 0.01f, 0.01f, 1.0f}};

        // Depth is needed only while rendering this frame.
        // We clear it before drawing and do not need to preserve it afterward.
        VkRenderingAttachmentInfo depthAttachment{};
        depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttachment.imageView = vrSwapChain->getDepthImageView(currentImageIndex);
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.clearValue.depthStencil = {1.0f, 0};

        // VkRenderingInfo replaces the per-frame information previously supplied
        // through VkRenderPassBeginInfo and VkFramebuffer.
        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent =
            vrSwapChain->getSwapChainExtent();
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;
        renderingInfo.pStencilAttachment = nullptr;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        // Viewport and scissor are dynamic pipeline states, so they are set while
        // recording commands instead of being fixed when the pipeline is created.
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vrSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vrSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{{0, 0}, vrSwapChain->getSwapChainExtent()};

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VrRenderer::endSwapChainRendering(VkCommandBuffer commandBuffer)
    {
        assert(
            isFrameStarted &&
            "Can't call endSwapChainRendering if frame is not in progress");
        assert(
            commandBuffer == getCurrentCommandBuffer() &&
            "Can't end rendering on command buffer from a different frame");

        vkCmdEndRendering(commandBuffer);

        // Rendering wrote the swap-chain image as a color attachment, but the
        // presentation engine requires it to be in PRESENT_SRC_KHR layout.
        transitionImageLayout(
            commandBuffer,
            vrSwapChain->getSwapChainImage(currentImageIndex),
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            VK_PIPELINE_STAGE_2_NONE,
            VK_ACCESS_2_NONE,
            VK_IMAGE_ASPECT_COLOR_BIT);

        // The depth image is not presented or used by another operation, so it can
        // remain in DEPTH_ATTACHMENT_OPTIMAL. A transition would be needed here
        // only if a later pass sampled or otherwise read the depth image.
    }

    void VrRenderer::transitionImageLayout(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkPipelineStageFlags2 srcStageMask,
        VkAccessFlags2 srcAccessMask,
        VkPipelineStageFlags2 dstStageMask,
        VkAccessFlags2 dstAccessMask,
        VkImageAspectFlags aspectMask)
    {
        // Describe how one image moves from its previous use/layout to its next
        // use/layout. The stage and access masks also establish the required
        // execution and memory dependency between those uses.
        VkImageMemoryBarrier2 imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        // Describe the image's previous use.
        imageBarrier.srcStageMask = srcStageMask;
        imageBarrier.srcAccessMask = srcAccessMask;

        // Describe the image's next use.
        imageBarrier.dstStageMask = dstStageMask;
        imageBarrier.dstAccessMask = dstAccessMask;

        imageBarrier.oldLayout = oldLayout;
        imageBarrier.newLayout = newLayout;

        // No queue-family ownership transfer is required.
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        imageBarrier.image = image;

        // Apply the transition to the selected color or depth subresource.
        imageBarrier.subresourceRange.aspectMask = aspectMask;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = 1;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;

        // Synchronization2 groups barriers inside VkDependencyInfo before recording
        // them into the command buffer.
        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageBarrier;

        // This is the command that performs the layout transition on the GPU.
        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }
} // namespace vr