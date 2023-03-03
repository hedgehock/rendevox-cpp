#include "vulkanWindow.hpp"
#include <cstdint>
#include <iostream>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

VulkanWindow::VulkanWindow(Rendevox::Window &window) {
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
                        &(const vk::ApplicationInfo &) vk::ApplicationInfo{
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
    } catch (vk::IncompatibleDriverError &error) {
        this->error("Vulkan Error: Failed to create instance! \'Incompatible Driver Error.");
    }

}

void VulkanWindow::getPhysicalDevice() {
    std::vector deviceList = this->instance->enumeratePhysicalDevices();

    std::cout << "\nPhysical devices:\n";

    for (vk::PhysicalDevice pd: deviceList) {
        vk::PhysicalDeviceProperties deviceProperties = pd.getProperties();
        std::cout << "    " << deviceProperties.deviceName << "\n";
    }

    for (vk::PhysicalDevice pd: deviceList) {
        if (VulkanWindow::isDeviceSuitable(pd)) {
            this->physicalDevice = pd;

            std::cout << "\nUsing GPU: " << pd.getProperties().deviceName << "\n\n";
            break;
        }
    }

}

void VulkanWindow::createLogicalDevice() {
}

void VulkanWindow::mainLoop() {

}

bool VulkanWindow::isDeviceSuitable(vk::PhysicalDevice device) {
    return (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu);
}

queueFamilyIndices VulkanWindow::findQueueFamilies(vk::PhysicalDevice device) {
    queueFamilyIndices indices = {0};
    uint32_t queueFamilyCount;
    device.vk::PhysicalDevice::getQueueFamilyProperties(&queueFamilyCount, nullptr, nullptr);
    vk::QueueFamilyProperties queueFamilies[queueFamilyCount];

    device.vk::PhysicalDevice::getQueueFamilyProperties(&queueFamilyCount, &queueFamilies, nullptr);

    for (int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.getGraphicsFamily = i;
            std::cout << "This GPU supports GRAPHICS queueFamilies!\n";
        }

    }
}

bool VulkanWindow::checkDeviceExtensionSupport(VkPhysicalDevice device) {

}

void VulkanWindow::error(const std::string &errorMessage) {
    std::cout << "Error: " << errorMessage << "\n";
    delete this;
    exit(EXIT_FAILURE);
}

VulkanWindow::~VulkanWindow() {

    this->logicalDevice->destroy();

    this->instance->destroy();

    std::cout << "Destructor has ended.";
}

/* https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/include/vulkanWindow.h
 * https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/src/vulkanWindow.c
 */
