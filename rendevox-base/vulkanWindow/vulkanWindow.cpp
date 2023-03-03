#include "../rendevox-base.hpp"

VulkanWindow::VulkanWindow(Rendevox::Window& window) {
    this->initVulkan();
    this->mainLoop();
}

void VulkanWindow::initVulkan() {
    this->createInstance();
    this->getPhysicalDevice();
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
                                VK_API_VERSION_1_0,
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

void VulkanWindow::getPhysicalDevice() {
    std::vector deviceList = this->instance->enumeratePhysicalDevices();

    std::cout << "\nPhysical devices:\n";

    for (vk::PhysicalDevice device: deviceList) {
        vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
        std::cout << "    " << deviceProperties.deviceName << ":\n";
    }

    if (!deviceList.empty()) {
        for (vk::PhysicalDevice device: deviceList) {
            if (VulkanWindow::isDeviceSuitable(device)) {
                this->physicalDevice = device;

                std::cout << "\nUsing GPU: " << device.getProperties().deviceName << "\n\n";
                break;
            }
        }

        if (std::find(deviceList.begin(), deviceList.end(), physicalDevice)->operator!=(physicalDevice)) {
            VulkanWindow::error("Rendevox Error: Failed to pick Physical device! \'Incompatible GPU.\'");
        }
    }

}

void VulkanWindow::createLogicalDevice() {
}

void VulkanWindow::mainLoop() {

}

bool VulkanWindow::isDeviceSuitable(vk::PhysicalDevice device) {
    queueFamilyIndices indices = VulkanWindow::findQueueFamilies(device);

    bool presentAllQueueFamilies = indices.isGraphicsFamilyPresent && indices.isPresentFamilyPresent;
    bool supportedGpuTypes = device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                             device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu;

    return (supportedGpuTypes && presentAllQueueFamilies);
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
            indices.isGraphicsFamilyPresent = true;
            std::cout << "        This GPU supports GRAPHICS queueFamilies!\n";
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
            std::cout << "        This GPU supports COMPUTE queueFamilies!\n";
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) {
            std::cout << "        This GPU supports TRANSFER queueFamilies!\n";
        }

        indices.isPresentFamilyPresent = true;

        if (indices.isGraphicsFamilyPresent && indices.isPresentFamilyPresent) {
            break;
        }

    }

    return indices;
}

bool VulkanWindow::checkDeviceExtensionSupport(VkPhysicalDevice device) {

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
