#ifndef VULKANWINDOW_HPP
#define VULKANWINDOW_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <GLFW/glfw3.h>

#include <window/window.hpp>

struct queueFamilyIndices {
    std::optional<uint32_t> getGraphicsFamily;
    std::optional<uint32_t> getPresentFamily;

    bool isComplete() {
        return getGraphicsFamily.has_value() || getPresentFamily.has_value();
    }
};

class VulkanWindow {
public:
    explicit VulkanWindow(Rendevox::Window &windowInfo);

    ~VulkanWindow();

private:
    vk::UniqueInstance instance;
    vk::UniqueSurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice logicalDevice;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    GLFWwindow* window{};

    void initWindow(Rendevox::Window& windowInfo);
    void initVulkan();
    void mainLoop();

    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();

    queueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
    static void printPhysicalDeviceInfo(vk::PhysicalDevice device);
    static std::vector<const char*> getRequiredExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(vk::PhysicalDevice device);
    static void error(const std::string& errorType, const std::string& errorMessage);
};

#endif
