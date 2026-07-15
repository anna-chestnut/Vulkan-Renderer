#include "vr_model.hpp"

#include "vr_utils.hpp"

//lib
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std {
template <>
struct hash<vr::VrModel::Vertex> {
  size_t operator()(vr::VrModel::Vertex const &vertex) const {
    size_t seed = 0;
    vr::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};
}  // namespace std

namespace vr {

VrModel::VrModel(VrDevice &device, const VrModel::Builder &builder) : vrDevice{device} {
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
}

VrModel::~VrModel() {
  vkDestroyBuffer(vrDevice.device(), vertexBuffer, nullptr);
  vkFreeMemory(vrDevice.device(), vertexBufferMemory, nullptr);

  if (hasIndexBuffer)
  {
    vkDestroyBuffer(vrDevice.device(), indexBuffer, nullptr);
    vkFreeMemory(vrDevice.device(), indexBufferMemory, nullptr);
  }
}

std::unique_ptr<VrModel> VrModel::createModelFromFile(VrDevice &device, const std::string &filepath)
{
  Builder builder{};
  builder.loadModel(filepath);
  return std::make_unique<VrModel>(device, builder);
}

void VrModel::createVertexBuffers(const std::vector<Vertex> &vertices)
{
  vertexCount = static_cast<uint32_t>(vertices.size());
  assert(vertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  vrDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // Host = CPU, Device = GPU
      stagingBuffer,
      stagingBufferMemory);

  void *data;
  vkMapMemory(vrDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(vrDevice.device(), stagingBufferMemory);

  vrDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      vertexBuffer,
      vertexBufferMemory);

  vrDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

  vkDestroyBuffer(vrDevice.device(), stagingBuffer, nullptr);
  vkFreeMemory(vrDevice.device(), stagingBufferMemory, nullptr);
}

void VrModel::createIndexBuffers(const std::vector<uint32_t> &indices) {
  indexCount = static_cast<uint32_t>(indices.size());
  hasIndexBuffer = indexCount > 0;
  if(!hasIndexBuffer) return;

  VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  vrDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // Host = CPU, Device = GPU
      stagingBuffer,
      stagingBufferMemory);

  void *data;
  vkMapMemory(vrDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
  vkUnmapMemory(vrDevice.device(), stagingBufferMemory);

  vrDevice.createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      indexBuffer,
      indexBufferMemory);

  vrDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

  vkDestroyBuffer(vrDevice.device(), stagingBuffer, nullptr);
  vkFreeMemory(vrDevice.device(), stagingBufferMemory, nullptr);
}

void VrModel::draw(VkCommandBuffer commandBuffer)
{
  if (hasIndexBuffer)
  {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  }
  else
  {

    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
  }
}

void VrModel::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (hasIndexBuffer)
  {
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  }
  
}

std::vector<VkVertexInputBindingDescription> VrModel::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VrModel::Vertex::getAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  
  attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
  attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
  attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
  attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
  
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);
  return attributeDescriptions;
}

void VrModel::Builder::loadModel(const std::string &filepath)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
  {
    throw std::runtime_error(warn + err);
  }

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto &shape : shapes)
  {
    for (const auto &index : shape.mesh.indices)
    {
      Vertex vertex{};

      if (index.vertex_index >= 0)
      {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        vertex.color = {
            attrib.colors[3 * index.vertex_index + 0],
            attrib.colors[3 * index.vertex_index + 1],
            attrib.colors[3 * index.vertex_index + 2],
        };
      }

      if (index.normal_index >= 0)
      {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
      }

      if (index.texcoord_index >= 0)
      {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1]
        };
      }

      if (uniqueVertices.count(vertex) == 0)
      {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(uniqueVertices[vertex]);      
    }
  }
}

} // namespace Vr