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

    [[nodiscard]] bool isComplete() const;
};

struct swapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class VulkanWindow {
public:
    explicit VulkanWindow(Rendevox::Window &windowInfo);

    ~VulkanWindow();

private:
    GLFWwindow* window{};

    vk::UniqueInstance instance;
    vk::UniqueSurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    vk::UniqueDevice logicalDevice;

    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    vk::UniqueSwapchainKHR swapChain;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat{};
    vk::Extent2D swapChainExtent;
    std::vector<vk::UniqueImageView> swapChainImagesViews;

    vk::UniquePipelineLayout pipelineLayout;

    void initWindow(Rendevox::Window& windowInfo);
    void initVulkan();
    void mainLoop();

    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();

    bool isDeviceSuitable(vk::PhysicalDevice device);

    queueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
    swapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    vk::UniqueShaderModule createShaderModule(const std::vector<char>& code);

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    static void printPhysicalDeviceInfo(vk::PhysicalDevice device);
    static std::vector<const char*> getRequiredExtensions();
    static bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
    static std::vector<char> readFile(const std::string& filename);
};

#endif
