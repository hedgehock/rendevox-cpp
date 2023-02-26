#ifndef VULKANWINDOW_HPP
#define VULKANWINDOW_HPP

#include <vulkan/vulkan.hpp>

#include "../window/window.hpp"

class VulkanWindow {
    public:
        VulkanWindow(Rendevox::Window& window);
        ~VulkanWindow();
    private:
        vk::UniqueInstance instance;
        void createInstance();
};

#endif
