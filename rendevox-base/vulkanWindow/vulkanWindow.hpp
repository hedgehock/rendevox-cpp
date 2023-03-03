#ifndef VULKANWINDOW_HPP
#define VULKANWINDOW_HPP

#include <vulkan/vulkan.hpp>

#include "../window/window.hpp"

typedef struct {
    uint32_t getGraphicsFamily;
    uint32_t getPresentFamily;
} queueFamilyIndices;

class VulkanWindow {
public:
    explicit VulkanWindow(Rendevox::Window &window);

    ~VulkanWindow();

private:
    vk::UniqueInstance instance;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice logicalDevice;

    void initVulkan();
    void mainLoop();

    void createInstance();
    void getPhysicalDevice();
    void createLogicalDevice();

    queueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    static bool isDeviceSuitable(vk::PhysicalDevice device);
    void error(const std::string& errorMessage);
};

#endif
