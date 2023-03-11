#ifndef VULKANWINDOW_HPP
#define VULKANWINDOW_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <window/window.hpp>

struct queueFamilyIndices {
    std::optional<uint32_t> getGraphicsFamily;

    bool isComplete() {
        return this->getGraphicsFamily.has_value();
    }
};

class VulkanWindow {
public:
    explicit VulkanWindow(Rendevox::Window &window);

    ~VulkanWindow();

private:
    vk::UniqueInstance instance;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice logicalDevice;
    vk::Queue graphicsQueue;

    void initVulkan();
    void mainLoop();

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();

    static queueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
    static void printPhysicalDeviceInfo(vk::PhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    static bool isDeviceSuitable(vk::PhysicalDevice device);
    static void error(const std::string& errorMessage);
};

#endif
