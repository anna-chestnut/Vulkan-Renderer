#pragma once

#include "vr_model.hpp"

// std
#include <memory>

namespace vr
{
    struct Transform2dComponent
    {
        glm::vec2 translation{}; // (position offset)
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2()
        {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};

            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
            return rotMatrix * scaleMat;
        }
    };

    class VrGameObject
    {
    public:
        using id_t = unsigned int;

        static VrGameObject createGameObject()
        {
            static id_t currentId = 0;
            return VrGameObject{currentId++};
        }

        VrGameObject(const VrGameObject &) = delete;
        VrGameObject &operator=(const VrGameObject &) = delete;
        VrGameObject(VrGameObject &&) = default;
        VrGameObject &operator=(VrGameObject &&) = default;

        const id_t getId() { return id; }

        std::shared_ptr<VrModel> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};

    private:
        VrGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
} // namespace vr
