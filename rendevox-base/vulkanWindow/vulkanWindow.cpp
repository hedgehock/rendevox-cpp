#include "../rendevox-base.hpp"

VulkanWindow::VulkanWindow(Rendevox::Window& window) {
    this->initVulkan();
    this->mainLoop();
}

void VulkanWindow::initVulkan() {
    this->createInstance();
    this->pickPhysicalDevice();
    this->createLogicalDevice();
}

void VulkanWindow::createInstance() {
    try {
        this->instance = vk::createInstanceUnique(
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
                        0,
                        nullptr,
                }
        );

        std::cout << "Instance was created\n";
    } catch (vk::IncompatibleDriverError& error) {
        VulkanWindow::error("Vulkan Error: Failed to create instance! \'Incompatible Driver Error.\'");
    }

}

void VulkanWindow::pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> deviceList = this->instance->enumeratePhysicalDevices();

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
                this->physicalDevice = device;
                break;
            }
        }

        if (std::find(deviceList.begin(), deviceList.end(), this->physicalDevice)->operator!=(this->physicalDevice)) {
            VulkanWindow::error("Rendevox Error: Failed to pick Physical device! \'Incompatible GPU.\'");
        } else {
            std::cout << "Using GPU: " << this->physicalDevice.getProperties().deviceName << "\n\n";
        }

    } else {
        VulkanWindow::error("No GPU found!");
    }

}

void VulkanWindow::createLogicalDevice() {
    queueFamilyIndices indices = VulkanWindow::findQueueFamilies(this->physicalDevice);

    float queuePriority = 1.0f;

    vk::DeviceQueueCreateInfo queueCreateInfos[1];

    queueCreateInfos[0] = vk::DeviceQueueCreateInfo{
            vk::DeviceQueueCreateFlags(),
            indices.getGraphicsFamily.value(),
            1,
            &queuePriority,
            nullptr


    };

    this->logicalDevice = physicalDevice.createDeviceUnique(
            vk::DeviceCreateInfo(
                    vk::DeviceCreateFlags(),
                    sizeof(queueCreateInfos) / sizeof(vk::DeviceQueueCreateInfo),
                    queueCreateInfos,
                    0, nullptr,
                    0, nullptr,
                    nullptr,
                    nullptr
            )
    );

    std::cout << "Logical device was created.\n";

    this->graphicsQueue = this->logicalDevice->getQueue(indices.getGraphicsFamily.value(), 0);


}

void VulkanWindow::mainLoop() {

}

bool VulkanWindow::isDeviceSuitable(vk::PhysicalDevice device) {
    queueFamilyIndices indices = VulkanWindow::findQueueFamilies(device);

    bool supportedGpuTypes = device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                             device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu;

    return (supportedGpuTypes && indices.isComplete());
}

queueFamilyIndices VulkanWindow::findQueueFamilies(vk::PhysicalDevice device) {
    queueFamilyIndices indices = {0};
    uint32_t queueFamilyCount;
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);
    vk::QueueFamilyProperties queueFamilies[queueFamilyCount];

    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies);

    for (int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.getGraphicsFamily = i;
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) {
        }

        if (indices.isComplete()) {
            break;
        }

    }

    return indices;
}

bool VulkanWindow::checkDeviceExtensionSupport(VkPhysicalDevice device) {

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
    std::cout << "            Images with linear tiling: " << std::string(
            formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no") << "\n";
    std::cout << "            Images with optimal tiling: " << std::string(
            formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no")
              << "\n";
    std::cout << "            Buffers: " << std::string(
            formatProperties.bufferFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no") << "\n";

}

void VulkanWindow::error(const std::string& errorMessage) {
    std::cout << "Error: " << errorMessage << "\n";
    exit(EXIT_FAILURE);
}

VulkanWindow::~VulkanWindow() {
    std::cout << "Destructor has ended.";
}

/* https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/include/vulkanWindow.h
 * https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/src/vulkanWindow.c
 */
