#ifndef RENDEVOXBASE_HPP
#define RENDEVOXBASE_HPP

#include <iostream>
#include <cstring>
#include <utility>
#include <string>
#include <optional>
#include <limits>

#include "glad/glad.hpp"

#include "vector2.hpp"
#include "vector3.hpp"
#include "vulkanException/VulkanError.hpp"
#include "window/window.hpp"
#include "openglWindow/openglWindow.hpp"
#include "openglRender/openglRender.hpp"
#include "vulkanWindow/vulkanWindow.hpp"

// User functions
void UserUpdate();

#endif
