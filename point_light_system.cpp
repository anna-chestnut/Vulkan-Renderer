#include "point_light_system.hpp"

namespace vr
{

    PointLightSystem::PointLightSystem() {};

    PointLightSystem::~PointLightSystem() {};

    void PointLightSystem::update(
    FrameInfo& frameInfo,
    GlobalUbo& ubo,
    std::vector<VrGameObject> &gameObjects) {

    for (auto &object : gameObjects) {
        if (!object.pointLight.has_value()) {
            continue;
        }

        ubo.lightPosition =
            glm::vec4(object.transform.translation, 1.f);

        ubo.lightColor =
            glm::vec4(
                object.color,
                object.pointLight->intensity);

        // Only supporting one light for now.
        break;
    }
}
    
} // namespace vr