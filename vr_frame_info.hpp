#pragma once

#include "vr_camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vr
{

    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, 0.02f};
        glm::vec4 lightPosition{0.f};
        glm::vec4 lightColor{1.f};
    };

    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VrCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
} // namespace vr