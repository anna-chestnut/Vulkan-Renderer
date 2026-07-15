#pragma once

#include "vr_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace vr
{
    class VrModel
    {
    public:
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex &other) const{
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Builder{
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string &filepath);
        };

        VrModel(VrDevice &device, const VrModel::Builder &builder);
        ~VrModel();

        VrModel(const VrModel &) = delete;
        VrModel &operator=(const VrModel &) = delete;

        static std::unique_ptr<VrModel> createModelFromFile(VrDevice &device, const std::string &filepath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        VrDevice &vrDevice;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
}