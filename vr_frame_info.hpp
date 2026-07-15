#pragma once

#include "vr_camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace vr
{
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VrCamera &camera;
    };
} // namespace vr