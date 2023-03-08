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

void VulkanWindow::pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> deviceList = this->instance->enumeratePhysicalDevices();

    std::cout << "\nPhysical devices:";

    if (!deviceList.empty()) {
        for (vk::PhysicalDevice device: deviceList) {

            vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
            std::cout << "\n    " << deviceProperties.deviceName << ":\n";

            if (VulkanWindow::isDeviceSuitable(device)) {
                this->physicalDevice = device;
                break;
            } else {
                std::cout << "        This GPU isn\'t suitable.";
            }

            std::cout << std::endl;
        }

        if (std::find(deviceList.begin(), deviceList.end(), this->physicalDevice)->operator!=(this->physicalDevice)) {
            VulkanWindow::error("Rendevox Error: Failed to pick Physical device! \'Incompatible GPU.\'");
        } else {
            std::cout << "\nUsing GPU: " << this->physicalDevice.getProperties().deviceName << "\n\n";
        }

    } else {
        VulkanWindow::error("No GPU found!");
    }

}

void VulkanWindow::createLogicalDevice() {
    queueFamilyIndices indices = VulkanWindow::findQueueFamilies(this->physicalDevice);

    float queuePriority = 1.0f;

    vk::DeviceQueueCreateInfo queueCreateInfos[2];

    queueCreateInfos[0] = vk::DeviceQueueCreateInfo{
        vk::DeviceQueueCreateFlags(),
        indices.getGraphicsFamily.value(),
        1,
        &queuePriority,
        nullptr


    };

    queueCreateInfos[1] = vk::DeviceQueueCreateInfo{
        vk::DeviceQueueCreateFlags(),
        indices.getPresentFamily.value(),
        1,
        &queuePriority,
        nullptr


    };

    try {
        this->logicalDevice = physicalDevice.createDeviceUnique(
            vk::DeviceCreateInfo(
                    vk::DeviceCreateFlags(),
                    queueCreateInfos,
                    nullptr,
                    nullptr,
                    nullptr
                    )
            );
    }

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
            std::cout << "        This GPU supports GRAPHICS queueFamilies at index " << i << "!\n";
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
            std::cout << "        This GPU supports COMPUTE queueFamilies at index " << i << "!\n";
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) {
            std::cout << "        This GPU supports TRANSFER queueFamilies at index " << i << "!\n";
        }

        indices.getPresentFamily = 0;

        if (indices.isComplete()) {
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

    this->logicalDevice->destroy();

    std::cout << "Destructor has ended.";
}

/* https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/include/vulkanWindow.h
 * https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/src/vulkanWindow.c
 */
