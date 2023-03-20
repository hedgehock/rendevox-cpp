#define GLFW_INCLUDE_VULKAN

#include <rendevox-base.hpp>

const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VulkanWindow::VulkanWindow(Rendevox::Window& windowInfo) {
    initWindow(windowInfo);
    initVulkan();
    mainLoop();
}

void VulkanWindow::initWindow(Rendevox::Window& windowInfo) {
    glfwInit();

    glfwWindowHint(
            GLFW_CONTEXT_VERSION_MAJOR,
            3);
    glfwWindowHint(
            GLFW_CONTEXT_VERSION_MINOR,
            3);
    glfwWindowHint(
            GLFW_CLIENT_API,
            GLFW_NO_API);
    glfwWindowHint(
            GLFW_RESIZABLE,
            GLFW_FALSE);

    if (windowInfo.fullscreen) {
        window = glfwCreateWindow(
                windowInfo.width,
                windowInfo.height,
                windowInfo.title,
                glfwGetPrimaryMonitor(),
                nullptr);
    } else {
        window = glfwCreateWindow(
                windowInfo.width,
                windowInfo.height,
                windowInfo.title,
                nullptr,
                nullptr);
    }

}

void VulkanWindow::initVulkan() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
}

void VulkanWindow::createInstance() {
    try {
        auto extensions = getRequiredExtensions();
        instance = vk::createInstanceUnique(
                vk::InstanceCreateInfo(
                        vk::InstanceCreateFlags(),
                        &(const vk::ApplicationInfo&) vk::ApplicationInfo(
                                "Rendevox-test",
                                VK_MAKE_VERSION(0,
                                                0,
                                                0),
                                "Rendevox",
                                VK_MAKE_VERSION(0,
                                                0,
                                                0),
                                VK_API_VERSION_1_3),
                        0,
                        nullptr,
                        static_cast<uint32_t>(extensions.size()),
                        extensions.data(),
                        nullptr));

        std::cout << "Instance was created\n";
    } catch (std::exception& error) {
        throw VulkanError("Failed to create instance!");
    }

}

void VulkanWindow::createSurface() {
    VkSurfaceKHR vkSurfaceKhrLocal;

    if (glfwCreateWindowSurface(
            instance->operator VkInstance(),
            window,
            nullptr,
            &vkSurfaceKhrLocal) != VK_SUCCESS) {
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

        if (std::find(
                deviceList.begin(),
                deviceList.end(),
                physicalDevice)->operator!=(physicalDevice)) {
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

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(0);
    std::vector<uint32_t> uniqueQueueFamilies = {
            indices.getGraphicsFamily.value(),
            indices.getPresentFamily.value()
    };

    for (uint32_t queueFamily: uniqueQueueFamilies) {
        float queuePriority = 1.0f;
        vk::DeviceQueueCreateInfo queueCreateInfo(
                vk::DeviceQueueCreateFlags(),
                queueFamily,
                1,
                &queuePriority,
                nullptr);
        queueCreateInfos.push_back(queueCreateInfo);
    }

    auto features = physicalDevice.getFeatures();

    try {
        logicalDevice = physicalDevice.createDeviceUnique(
                vk::DeviceCreateInfo(
                        vk::DeviceCreateFlags(),
                        queueCreateInfos.size(),
                        queueCreateInfos.data(),
                        0,
                        nullptr,
                        deviceExtensions.size(),
                        deviceExtensions.data(),
                        &features,
                        nullptr));
    } catch (std::exception& error) {
        throw VulkanError("Cannot create logical device!");
    }

    std::cout << "Logical device was created.\n";

    graphicsQueue = logicalDevice->getQueue(
            indices.getGraphicsFamily.value(),
            0);
    presentQueue = logicalDevice->getQueue(
            indices.getPresentFamily.value(),
            0);

}

void VulkanWindow::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

}

bool VulkanWindow::isDeviceSuitable(vk::PhysicalDevice device) {
    queueFamilyIndices indices = findQueueFamilies(device);
    auto features = device.getFeatures();
    bool swapChainAdequate = false;
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool supportedGpuTypes = device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                             device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu;
    bool otherOptions = features.geometryShader;

    if (extensionsSupported) {
        swapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return (supportedGpuTypes && indices.isComplete() && extensionsSupported && swapChainAdequate && otherOptions);
}

queueFamilyIndices VulkanWindow::findQueueFamilies(vk::PhysicalDevice device) {
    queueFamilyIndices indices{};
    uint32_t queueFamilyCount;
    device.getQueueFamilyProperties(
            &queueFamilyCount,
            nullptr);
    vk::QueueFamilyProperties queueFamilies[queueFamilyCount];
    VkBool32 presentSupport = false;

    device.getQueueFamilyProperties(
            &queueFamilyCount,
            queueFamilies);

    for (int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.getGraphicsFamily = i;
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
        }

        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) {
        }

        if (device.getSurfaceSupportKHR(
                i,
                surface->operator VkSurfaceKHR(),
                &presentSupport) != vk::Result::eSuccess) {
            throw VulkanError("Cannot check KHR surface support!");
        }

        if (presentSupport) {
            indices.getPresentFamily = i;
        }

        if (indices.isComplete() && indices.getGraphicsFamily != indices.getPresentFamily) {
            break;
        }

    }

    return indices;
}

bool VulkanWindow::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
    uint32_t extensionCount;
    if (device.enumerateDeviceExtensionProperties(
            nullptr,
            &extensionCount,
            nullptr) != vk::Result::eSuccess) {
        throw VulkanError("Cannot get extension count!");
    }

    std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);

    if (device.enumerateDeviceExtensionProperties(
            nullptr,
            &extensionCount,
            availableExtensions.data()) != vk::Result::eSuccess) {
        throw VulkanError("Cannot get extension names!");
    }

    int supportedExtensionCount = 0;

    for (auto deviceExtension: deviceExtensions) {
        for (int e = 0; e < extensionCount; e++) {
            if (strcmp(
                    deviceExtension,
                    availableExtensions[e].extensionName) == 0) {
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
    for (int i = 0; i < uint32_t(queueFamilyList.size()); i++) {
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
    std::cout << "            Images with linear tiling: "
              << (formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no")
              << "\n";
    std::cout << "            Images with optimal tiling: "
              << (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no")
              << "\n";
    std::cout << "            Buffers: "
              << (formatProperties.bufferFeatures & vk::FormatFeatureFlagBits::eColorAttachment ? "yes" : "no") << "\n";

}

std::vector<const char*> VulkanWindow::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(
            glfwExtensions,
            glfwExtensions + glfwExtensionCount);

    return extensions;
}

swapChainSupportDetails VulkanWindow::querySwapChainSupport(vk::PhysicalDevice device) {
    swapChainSupportDetails details;
    uint32_t formatCount;
    uint32_t presentModeCount;

    if (device.getSurfaceCapabilitiesKHR(
            *surface,
            &details.capabilities) != vk::Result::eSuccess) {
        throw VulkanError("Cannot get KHR surface Capabilities!");
    }

    if (device.getSurfaceFormatsKHR(
            *surface,
            &formatCount,
            nullptr) != vk::Result::eSuccess) {
        throw VulkanError("Cannot get number of KHR surface formats!");
    }

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        if (device.getSurfaceFormatsKHR(
                *surface,
                &formatCount,
                details.formats.data()) != vk::Result::eSuccess) {
            throw VulkanError("Cannot get KHR surface formats!");
        }
    }

    if (device.getSurfacePresentModesKHR(
            *surface,
            &presentModeCount,
            nullptr) != vk::Result::eSuccess) {
        throw VulkanError("Cannot get number of present modes!");
    }

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        if (device.getSurfacePresentModesKHR(
                *surface,
                &presentModeCount,
                details.presentModes.data()) != vk::Result::eSuccess) {
            throw VulkanError("Cannot get present modes!");
        }
    }

    return details;
}

vk::SurfaceFormatKHR VulkanWindow::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat: availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR VulkanWindow::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode: availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanWindow::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(
                window,
                &width,
                &height);

        vk::Extent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(
                actualExtent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(
                actualExtent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanWindow::createSwapChain() {
    swapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    createInfo.surface = surface->operator VkSurfaceKHR();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    queueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {
            indices.getGraphicsFamily.value(),
            indices.getPresentFamily.value()
    };

    if (indices.getGraphicsFamily != indices.getPresentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    try {
        swapChain = logicalDevice->createSwapchainKHRUnique(
                createInfo,
                nullptr);
    } catch (std::exception& error) {
        throw VulkanError("Cannot create swap chain!");
    }

    if (logicalDevice.get().getSwapchainImagesKHR(
            swapChain->operator VkSwapchainKHR(),
            &imageCount,
            nullptr) != vk::Result::eSuccess) {
        throw VulkanError("Failed to get swap chain KHR image count.");
    }
    swapChainImages.resize(imageCount);
    if (logicalDevice->getSwapchainImagesKHR(
            swapChain->operator VkSwapchainKHR(),
            &imageCount,
            swapChainImages.data()) != vk::Result::eSuccess) {
        throw VulkanError("Failed to get swap chain KHR images.");
    }

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

}

void VulkanWindow::createImageViews() {
    swapChainImagesViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        try {
            swapChainImagesViews[i] = logicalDevice->createImageViewUnique(
                    vk::ImageViewCreateInfo(
                            vk::ImageViewCreateFlags(),
                            swapChainImages[i],
                            vk::ImageViewType::e2D,
                            swapChainImageFormat,
                            vk::ComponentMapping(
                                    vk::ComponentSwizzle::eIdentity,
                                    vk::ComponentSwizzle::eIdentity,
                                    vk::ComponentSwizzle::eIdentity,
                                    vk::ComponentSwizzle::eIdentity),
                            vk::ImageSubresourceRange(
                                    vk::ImageAspectFlagBits::eColor,
                                    0,
                                    1,
                                    0,
                                    1)));
        } catch (std::exception& error) {
            throw VulkanError(
                    fmt::format(
                            "Failed to get swap chain image view at index {}!",
                            i).c_str());
        }
    }
}

void VulkanWindow::createGraphicsPipeline() {
    auto vertShaderCode = readFile("rendevox-base/vulkanWindow/shaders/shader.vert.spv");
    auto fragShaderCode = readFile("rendevox-base/vulkanWindow/shaders/shader.frag.spv");

    vk::UniqueShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    vk::UniqueShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
            vk::PipelineShaderStageCreateInfo(
                    vk::PipelineShaderStageCreateFlags(),
                    vk::ShaderStageFlagBits::eVertex,
                    vertShaderModule.operator*(),
                    "main",
                    nullptr),
            vk::PipelineShaderStageCreateInfo(
                    vk::PipelineShaderStageCreateFlags(),
                    vk::ShaderStageFlagBits::eFragment,
                    fragShaderModule.operator*(),
                    "main",
                    nullptr)
    };

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo(
            vk::PipelineVertexInputStateCreateFlags(),
            0,
            nullptr,
            0,
            nullptr);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly = vk::PipelineInputAssemblyStateCreateInfo(
            vk::PipelineInputAssemblyStateCreateFlags(),
            vk::PrimitiveTopology::eTriangleList,
            VK_FALSE);

    vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo(
            vk::PipelineViewportStateCreateFlags(),
            1,
            nullptr,
            1,
            nullptr);

    vk::PipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = VK_FALSE;

    vk::PipelineColorBlendStateCreateInfo colorBlending = vk::PipelineColorBlendStateCreateInfo(
            vk::PipelineColorBlendStateCreateFlags(),
            VK_FALSE,
            vk::LogicOp::eCopy,
            1,
            &colorBlendAttachment,
            {
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f
            });

    std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState = vk::PipelineDynamicStateCreateInfo(
            vk::PipelineDynamicStateCreateFlags(),
            static_cast<uint32_t>(dynamicStates.size()),
            dynamicStates.data());

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(
            vk::PipelineLayoutCreateFlags(),
            0,
            nullptr,
            0,
            nullptr);

    try {
        pipelineLayout = logicalDevice->createPipelineLayoutUnique(pipelineLayoutInfo);
    } catch (std::exception& error) {
        throw VulkanError("Failed to create pipeline layout!");
    }

    vertShaderModule.release();
    fragShaderModule.release();
}

std::vector<char> VulkanWindow::readFile(const std::string& filename) {
    std::ifstream file(
            filename,
            std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw RendevoxError("Failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(
            buffer.data(),
            std::streamsize(fileSize));

    file.close();

    return buffer;
}

vk::UniqueShaderModule VulkanWindow::createShaderModule(const std::vector<char>& code) {
    vk::UniqueShaderModule shaderModule;
    try {
        shaderModule = logicalDevice->createShaderModuleUnique(
                vk::ShaderModuleCreateInfo(
                        vk::ShaderModuleCreateFlags(),
                        code.size(),
                        reinterpret_cast<const uint32_t*>(code.data()),
                        nullptr));
    } catch (std::exception& error) {
        throw VulkanError("Failed to create shader module!");
    }

    return shaderModule;
}

void VulkanWindow::createRenderPass() {
    vk::AttachmentDescription colorAttachment = vk::AttachmentDescription(
            vk::AttachmentDescriptionFlags(),
            swapChainImageFormat,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference(
            0,
            vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    try {
        renderPass = logicalDevice->createRenderPassUnique(
                vk::RenderPassCreateInfo(
                        vk::RenderPassCreateFlags(),
                        1,
                        &colorAttachment,
                        1,
                        &subpass));
    } catch (std::exception& error) {
        throw VulkanError("Failed to create render pass!");
    }
}

VulkanWindow::~VulkanWindow() {
    pipelineLayout.release();

    renderPass.release();

    swapChainImagesViews.data()->release();

    swapChain.release();

    surface.release();

    glfwDestroyWindow(window);

    glfwTerminate();

    std::cout << "Destructor has ended.";
}

bool queueFamilyIndices::isComplete() const {
    return getGraphicsFamily.has_value() && getPresentFamily.has_value();
}

/* https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/include/vulkanWindow.h
 * https://github.com/hedgehock/rendevox/blob/vulkan/rendevox/src/vulkanWindow.c
 */
