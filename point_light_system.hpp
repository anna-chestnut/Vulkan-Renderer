#pragma once

#include "vr_frame_info.hpp"
#include "vr_game_object.hpp"

//std
#include <vector>

namespace vr
{
    class PointLightSystem{

    public:
        PointLightSystem();
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void update(FrameInfo &frameInfo, GlobalUbo &ubo, std::vector<VrGameObject> &gameObjects);

    private:
    };

} // namespace vr
