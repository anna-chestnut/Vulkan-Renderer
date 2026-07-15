#pragma once

#include "vr_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace vr
{
    struct TransformComponent
    {
        glm::vec3 translation{}; // (position offset)
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();    
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
        TransformComponent transform{};

    private:
        VrGameObject(id_t objId) : id{objId} {}

        id_t id;
    };
} // namespace vr
