#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vr_buffer.hpp"
#include "vr_camera.hpp"
#include "simple_render_system.hpp"
#include "point_light_system.hpp"
#include "vr_frame_info.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>

namespace vr{

    FirstApp::FirstApp()
    {
        globalPool = VrDescriptorPool::Builder(vrDevice)
                         .setMaxSets(VrSwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VrSwapChain::MAX_FRAMES_IN_FLIGHT)
                         .build();
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run(){
        std::vector<std::unique_ptr<VrBuffer>> uboBuffers(VrSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<VrBuffer>(
                vrDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = VrDescriptorSetLayout::Builder(vrDevice)
                                   .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
                                   .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(VrSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < globalDescriptorSets.size(); i++){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            VrDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{
            vrDevice,
            vrRenderer.getSwapChainImageFormat(),
            vrRenderer.getSwapChainDepthFormat(),
            globalSetLayout->getDescriptorSetLayout()};
        VrCamera camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        // Invisible helper object that stores the camera's position and rotation.
        // It is not rendered; its transform is used to update the camera.
        auto viewObject = VrGameObject::createGameObject();
        KeyboardMovementController cameraController{};
        
        PointLightSystem pointLightSystem{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!vrWindow.shouldClose()){

            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(vrWindow.getGLFWwindow(), frameTime, viewObject);
            camera.setViewYXZ(viewObject.transform.translation, viewObject.transform.rotation);
            
            float aspect = vrRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
            if(auto commandBuffer  = vrRenderer.beginFrame()){

                int frameIndex = vrRenderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();

                ubo.ambientLightColor = glm::vec4{1.f, 1.f, 1.f, 0.02f};
                // Fill all CPU-side UBO values first.
                pointLightSystem.update(frameInfo, ubo, gameObjects);

                // Upload the completed UBO afterward.
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                vrRenderer.beginSwapChainRendering(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                vrRenderer.endSwapChainRendering(commandBuffer);
                vrRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(vrDevice.device());
    }

void FirstApp::loadGameObjects()
{
    std::shared_ptr<VrModel> VrModel =
        VrModel::createModelFromFile(vrDevice, "models/flat_vase.obj");
    //   auto flatVase = VrGameObject::createGameObject();
    //   flatVase.model = VrModel;
    //   flatVase.transform.translation = {-.5f, .5f, 2.5f};
    //   flatVase.transform.scale = {3.f, 1.5f, 3.f};
    //   gameObjects.push_back(std::move(flatVase));

    VrModel = VrModel::createModelFromFile(vrDevice, "models/smooth_vase.obj");
    auto smoothVase = VrGameObject::createGameObject();
    smoothVase.model = VrModel;
    smoothVase.transform.translation = {-1.0f, .5f, 2.5f};
    smoothVase.transform.scale = {3.f, 1.5f, 3.f};
    gameObjects.push_back(std::move(smoothVase));

     VrModel = VrModel::createModelFromFile(vrDevice, "models/colored_cube.obj");
    auto lightObject = VrGameObject::createGameObject();

    lightObject.model = VrModel;
    lightObject.transform.translation = {.5f, .8f, 1.f};
    lightObject.color = {1.f, 1.f, 1.f};
    lightObject.transform.scale = {0.1f, 0.1f, 0.1f};
    lightObject.pointLight = PointLightComponent{
        15.f, // intensity
        0.5f  // radius
    };
    gameObjects.push_back(std::move(lightObject));
}
}