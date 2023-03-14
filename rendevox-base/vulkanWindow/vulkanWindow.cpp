#define GLFW_INCLUDE_VULKAN

#include <rendevox-base.hpp>

const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanWindow::VulkanWindow(Rendevox::Window& windowInfo) {
    initWindow(windowInfo);
    initVulkan();
    mainLoop();
}

void VulkanWindow::initWindow(Rendevox::Window& windowInfo) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if (windowInfo.fullscreen) {
        window = glfwCreateWindow(windowInfo.width, windowInfo.height, windowInfo.title, glfwGetPrimaryMonitor(),
                                  nullptr);
    } else {
        window = glfwCreateWindow(windowInfo.width, windowInfo.height, windowInfo.title, nullptr, nullptr);
    }

}

void VulkanWindow::initVulkan() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}

void VulkanWindow::createInstance() {
    try {
        auto extensions = getRequiredExtensions();
        instance = vk::createInstanceUnique(
                vk::InstanceCreateInfo{
                        vk::InstanceCreateFlags(),
                        &(const vk::ApplicationInfo&) vk::ApplicationInfo{
                                "Rendevox-test",
                                VK_MAKE_VERSION(0, 0, 0),
                                "Rendevox",
                                VK_MAKE_VERSION(0, 0, 0),
                                VK_API_VERSION_1_3,
                        },
                        0,
                        nullptr,
                        static_cast<uint32_t>(extensions.size()),
                        extensions.data(),
                        nullptr
                }
        );

        std::cout << "Instance was created\n";
    } catch (std::exception& error) {
        throw VulkanError("Failed to create instance!");
    }

}

void VulkanWindow::createSurface() {
    VkSurfaceKHR vkSurfaceKhrLocal;

    if (glfwCreateWindowSurface(instance->operator VkInstance(), window, nullptr, &vkSurfaceKhrLocal) != VK_SUCCESS) {
        throw GLFWError("Failed to create window surface.");
    }

    surface = vk::UniqueSurfaceKHR(vkSurfaceKhrLocal);
}

void VulkanWindow::pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> deviceList = instance->enumeratePhysicalDevices();

    if (!deviceList.empty()) {

        std::cout << "\nPhysical devices:\n";

        for (vk::PhysicalDevice device: deviceList) {

            VulkanWindow::printPhysicalDeviceInfo(device);

            if (VulkanWindow::isDeviceSuitable(device)) {
                std::cout << "        This GPU is suitable.\n";
            } else {
                std::cout << "        This GPU isn\'t suitable.\n";
            }

            std::cout << "\n";
        }

        for (vk::PhysicalDevice device: deviceList) {
            if (VulkanWindow::isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (std::find(deviceList.begin(), deviceList.end(), physicalDevice)->operator!=(physicalDevice)) {
            throw RendevoxError("Failed to pick Physical device! \'Incompatible GPU.\'");
        } else {
            std::cout << "Using GPU: " << physicalDevice.getProperties().deviceName << "\n\n";
        }

    } else {
        throw IncompatibilityError("No GPU found!");
    }

}

void VulkanWindow::createLogicalDevice() {
    queueFamilyIndices indices = VulkanWindow::findQueueFamilies(physicalDevice);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::vector<uint32_t> uniqueQueueFamilies = {indices.getGraphicsFamily.value(), indices.getPresentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo(
                vk::DeviceQueueCreateFlags(),
                queueFamily,
                1,
                &queuePriority,
                nullptr
        );
        queueCreateInfos.push_back(queueCreateInfo);
    }

    try {
        logicalDevice = physicalDevice.createDeviceUnique(
                vk::DeviceCreateInfo(
                        vk::DeviceCreateFlags(),
                        queueCreateInfos.size(),
                        queueCreateInfos.data(),
                        0, nullptr,
                        deviceExtensions.size(), deviceExtensions.data(),
                        nullptr,
                        nullptr
                )
        );
    } catch (std::exception& error) {
        throw VulkanError("Cannot create logical device!");
    }

    std::cout << "Logical device was created.\n";

    graphicsQueue = logicalDevice->getQueue(indices.getGraphicsFamily.value(), 0);
    presentQueue = logicalDevice->getQueue(indices.getPresentFamily.value(), 0);

}

void VulkanWindow::mainLoop() {

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

}

bool VulkanWindow::isDeviceSuitable(vk::PhysicalDevice device) {
    queueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool supportedGpuTypes = device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                             device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu;

    return (supportedGpuTypes && indices.isComplete() && extensionsSupported);
}

queueFamilyIndices VulkanWindow::findQueueFamilies(vk::PhysicalDevice device) {
    queueFamilyIndices indices = {0};
    uint32_t queueFamilyCount;
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);
    vk::QueueFamilyProperties queueFamilies[queueFamilyCount];
    VkBool32 presentSupport = false;

    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies);

    for (int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.getGraphicsFamily = i;
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) {
        }

        if (device.getSurfaceSupportKHR(i, surface->operator VkSurfaceKHR(), &presentSupport) != vk::Result::eSuccess) {
            throw VulkanError("Cannot check KHR surface support!");
        }

        if (presentSupport) {
            indices.getPresentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

    }

    return indices;
}

bool VulkanWindow::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
    uint32_t extensionCount;
    if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr) != vk::Result::eSuccess) {
        throw VulkanError("Cannot get extension count!");
    }

    std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);

    if (device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) !=
        vk::Result::eSuccess) {
        throw VulkanError("Cannot get extension names!");
    }

    int supportedExtensionCount = 0;

    for (auto deviceExtension: deviceExtensions) {
        for (int e = 0; e < extensionCount; e++) {
            if (strcmp(deviceExtension, availableExtensions[e].extensionName) == 0) {
                supportedExtensionCount++;
            }
        }
    }

    return supportedExtensionCount == int(deviceExtensions.size());
}

void VulkanWindow::printPhysicalDeviceInfo(vk::PhysicalDevice device) {
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
    vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = device.getMemoryProperties();
    std::vector<vk::QueueFamilyProperties> queueFamilyList = device.getQueueFamilyProperties();
    vk::FormatProperties formatProperties = device.getFormatProperties(vk::Format::eR8G8B8A8Unorm);

    std::cout << "    " << deviceProperties.deviceName << ":\n";
    std::cout << "        Vulkan version: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion) << "." << VK_VERSION_PATCH(deviceProperties.apiVersion)
              << "\n";
    std::cout << "        Max Texture Size: " << deviceProperties.limits.maxImageDimension2D << "\n";
    std::cout << "        Geometry Shader: " << (deviceFeatures.geometryShader ? "supported" : "not supported") << "\n";
    std::cout << "        Memory heaps:\n";
    for (int i = 0; i < deviceMemoryProperties.memoryHeapCount; i++) {
        std::cout << "            " << i << ": " << deviceMemoryProperties.memoryHeaps[i].size / 1024 / 1024 << "MiB\n";
    }
    std::cout << "        Queue families:\n";
    for (int i = 0; i < uint32_t(queueFamilyList.size()); ++i) {
        std::cout << "            " << i << ": ";
        if (queueFamilyList[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            std::cout << "g";
        }
        if (queueFamilyList[i].queueFlags & vk::QueueFlagBits::eCompute) {
            std::cout << "c";
        }
        if (queueFamilyList[i].queueFlags & vk::QueueFlagBits::eTransfer) {
            std::cout << "t";
        }
        std::cout << "  (count: " << queueFamilyList[i].queueCount << ")\n";
    }
    std::cout << "        R8G8B8A8Unorm format support for color attachment:\n";
    std::cout << "            Images with linear tiling: " << (
            formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no") << "\n";
    std::cout << "            Images with optimal tiling: " << (
            formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no")
              << "\n";
    std::cout << "            Buffers: " << (
            formatProperties.bufferFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no") << "\n";

}

std::vector<const char*> VulkanWindow::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}

VulkanWindow::~VulkanWindow() {
    glfwDestroyWindow(window);

    glfwTerminate();

    surface.release();

    std::cout << "Destructor has ended.";
}

/* https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/include/vulkanWindow.h
 * https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/src/vulkanWindow.c
 */
