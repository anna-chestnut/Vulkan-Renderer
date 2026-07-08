#include "vr_renderer.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vr {

VrRenderer::VrRenderer(VrWindow& window, VrDevice& device)
    : vrWindow{window}, vrDevice{device} {
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

void VrRenderer::createCommandBuffers() {
  commandBuffers.resize(VrSwapChain::MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = vrDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(vrDevice.device(), &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void VrRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(
      vrDevice.device(),
      vrDevice.getCommandPool(),
      static_cast<uint32_t>(commandBuffers.size()),
      commandBuffers.data());
  commandBuffers.clear();
}

VkCommandBuffer VrRenderer::beginFrame() {
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

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
  return commandBuffer;
}

void VrRenderer::endFrame() {
  assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

  // End command buffer
  auto commandBuffer = getCurrentCommandBuffer();
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  // Submit the finished command buffer to the graphics queue and present the image.
  // If the swap chain is no longer compatible with the window, such as after resize
  // or because Vulkan reports it as out of date/suboptimal, recreate the swap chain
  // before continuing to the next frame.
  auto result = vrSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      vrWindow.wasWindowResized()) {
    vrWindow.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  isFrameStarted = false;
  currentFrameIndex = (currentFrameIndex + 1) % VrSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VrRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
  assert(
      commandBuffer == getCurrentCommandBuffer() &&
      "Can't begin render pass on command buffer from a different frame");

  // Begin the render pass for the current swap chain image.
  // This tells Vulkan which framebuffer to render into, what area to render,
  // and what color/depth values to clear before drawing.
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vrSwapChain->getRenderPass();
  renderPassInfo.framebuffer = vrSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = vrSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  // Set dynamic viewport and scissor to match the current swap chain extent.
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

void VrRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
  assert(
      commandBuffer == getCurrentCommandBuffer() &&
      "Can't end render pass on command buffer from a different frame");
  vkCmdEndRenderPass(commandBuffer);
}

}  // namespace vr