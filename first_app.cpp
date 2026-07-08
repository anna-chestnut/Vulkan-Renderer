#include "first_app.hpp"

#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vr{

    FirstApp::FirstApp()
    {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run(){
        SimpleRenderSystem simpleRenderSystem{vrDevice, vrRenderer.getSwapChainRenderPass()};

        while(!vrWindow.shouldClose()){

            glfwPollEvents();
            if(auto commandBuffer  = vrRenderer.beginFrame()){

                vrRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                vrRenderer.endSwapChainRenderPass(commandBuffer);
                vrRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(vrDevice.device());
    }


    void FirstApp::loadGameObjects()
    {
        std::vector<VrModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        auto vrModel = std::make_shared<VrModel>(vrDevice, vertices);

        auto triangle = VrGameObject::createGameObject();
        triangle.model = vrModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}