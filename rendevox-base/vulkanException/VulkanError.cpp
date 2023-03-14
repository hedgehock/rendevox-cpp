#include "VulkanError.hpp"

VulkanError::VulkanError(const char* errorMessage): msg_(errorMessage) {
}

const char* VulkanError::what() const noexcept {
    return msg_;
}

GLFWError::GLFWError(const char* errorMessage): msg_(errorMessage) {
}

const char* GLFWError::what() const noexcept {
    return msg_;
}

IncompatibilityError::IncompatibilityError(const char* errorMessage): msg_(errorMessage) {
}

const char* IncompatibilityError::what() const noexcept {
    return msg_;
}

RendevoxError::RendevoxError(const char* errorMessage): msg_(errorMessage) {
}

const char* RendevoxError::what() const noexcept {
    return msg_;
}
