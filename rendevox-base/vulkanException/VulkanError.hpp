#ifndef RENDEVOX_VULKANERROR_HPP
#define RENDEVOX_VULKANERROR_HPP

#include <iostream>

class VulkanError : public std::exception {
public:
    explicit VulkanError(const char* errorMessage);

    const char* what() const noexcept override;

private:
    const char* msg_;
};

class GLFWError : public std::exception {
public:
    explicit GLFWError(const char* errorMessage);

    const char* what() const noexcept override;

private:
    const char* msg_;
};

class IncompatibilityError : public std::exception {
public:
    explicit IncompatibilityError(const char* errorMessage);

    const char* what() const noexcept override;

private:
    const char* msg_;
};

class RendevoxError : public std::exception {
public:
    explicit RendevoxError(const char* errorMessage);

    const char* what() const noexcept override;

private:
    const char* msg_;
};

#endif
