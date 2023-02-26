#include <vulkan/vulkan.hpp>

vk::UniqueInstance instance (
    vk::createInstanceUnique (
        vk::InstanceCreateInfo {
            vk::InstanceCreateFlags(),
            & (const vk::ApplicationInfo&)vk::ApplicationInfo {
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
    )
);
