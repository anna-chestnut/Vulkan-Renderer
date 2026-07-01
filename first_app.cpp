#include "first_app.hpp"


// std
#include <array>
#include <stdexcept>

namespace vr{

    FirstApp::FirstApp()
    {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() { vkDestroyPipelineLayout(vrDevice.device(), pipelineLayout, nullptr); }

    void FirstApp::run(){
        while(!vrWindow.shouldClose()){
            glfwPollEvents();
        }
    }

    void FirstApp::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(vrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline()
    {
        PipelineConfigInfo pipelineConfig =
        VrPipeline::defaultPipelineConfigInfo(
            vrSwapChain.width(),
            vrSwapChain.height());
        pipelineConfig.renderPass = vrSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        vrPipeline = std::make_unique<VrPipeline>(
            vrDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::createCommandBuffers()
    {
        
    }
    void FirstApp::drawFrame()
    {
        
    }
}