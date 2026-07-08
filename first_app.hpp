#pragma once

#include "vr_window.hpp"
#include "vr_device.hpp"
#include "vr_game_object.hpp"
#include "vr_renderer.hpp"

//std
#include <memory>
#include <vector>

namespace vr
{
    class FirstApp{

        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void loadGameObjects();

        VrWindow vrWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VrDevice vrDevice{vrWindow};
        VrRenderer vrRenderer{vrWindow, vrDevice};
        
        std::vector<VrGameObject> gameObjects;
    };

} // namespace vr
