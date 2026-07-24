#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "vr_buffer.hpp"
#include "vr_camera.hpp"
#include "simple_render_system.hpp"

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

    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

    FirstApp::FirstApp()
    {
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

        SimpleRenderSystem simpleRenderSystem{vrDevice, vrRenderer.getSwapChainImageFormat(), vrRenderer.getSwapChainDepthFormat()};
        VrCamera camera{};
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));
        // Invisible helper object that stores the camera's position and rotation.
        // It is not rendered; its transform is used to update the camera.
        auto viewObject = VrGameObject::createGameObject();
        KeyboardMovementController cameraController{};

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
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera};

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
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
  auto flatVase = VrGameObject::createGameObject();
  flatVase.model = VrModel;
  flatVase.transform.translation = {-.5f, .5f, 2.5f};
  flatVase.transform.scale = {3.f, 1.5f, 3.f};
  gameObjects.push_back(std::move(flatVase));

  VrModel = VrModel::createModelFromFile(vrDevice, "models/smooth_vase.obj");
  auto smoothVase = VrGameObject::createGameObject();
  smoothVase.model = VrModel;
  smoothVase.transform.translation = {.5f, .5f, 2.5f};
  smoothVase.transform.scale = {3.f, 1.5f, 3.f};
  gameObjects.push_back(std::move(smoothVase));
}
}