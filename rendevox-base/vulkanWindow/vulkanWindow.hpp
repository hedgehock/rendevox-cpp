#ifndef VULKANWINDOW_HPP
#define VULKANWINDOW_HPP

#include <vulkan/vulkan.hpp>

#include "../window/window.hpp"

class VulkanWindow {
    private:
        vk::UniqueInstance instance;
};

#endif
