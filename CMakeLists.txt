cmake_minimum_required(VERSION 3.23)
project(rendevox CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    link_libraries(glfw vulkan m)
elseif(UNIX)
    link_libraries(glfw vulkan m)
elseif(WIN32)
    find_package(Vulkan REQUIRED)
    find_package(OpenAL REQUIRED)

    include_directories(/c/msys64/mingw64/include)
    link_directories(/c/msys64/mingw64/lib)
    link_libraries(mingw32 Vulkan::Vulkan OpenAL::OpenAL glfw3 glu32 opengl32 gdi32 m)
endif()

file(GLOB SRC
        rendevox-base/glad/glad.cpp
        rendevox-base/window/window.cpp
        rendevox-base/openglWindow/openglWindow.cpp
        rendevox-base/openglRender/openglRender.cpp
        rendevox-base/vulkanWindow/vulkanWindow.cpp)

add_library(rendevoxlib STATIC ${SRC})

include_directories(rendevox-base)

add_executable(rendevox app/main.cpp)

target_link_libraries(rendevox rendevoxlib)
