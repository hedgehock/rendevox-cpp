cmake_minimum_required(VERSION 3.23)
project(rendevox CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

function(compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT" "SOURCES")
    foreach (source ${arg_SOURCES})
        add_custom_command(
                OUTPUT ${source}.${arg_FORMAT}
                DEPENDS ${source}
                DEPFILE ${source}.d
                COMMAND
                ${glslc_executable}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                $<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                -MD -MF ${source}.spv
                -o ${source}.${arg_FORMAT}
                ${CMAKE_CURRENT_SOURCE_DIR}/${source}
        )
        target_sources(${target} PRIVATE ${source}.${arg_FORMAT})
    endforeach ()
endfunction()


#https://stackoverflow.com/questions/60420700/cmake-invocation-of-glslc-with-respect-to-includes-dependencies


if (APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
    link_libraries(glfw vulkan m)
elseif (UNIX)
    link_libraries(glfw vulkan m)
elseif (WIN32)
    find_package(Vulkan REQUIRED)
    find_package(OpenAL REQUIRED)
    find_package(fmt REQUIRED)

    include_directories(/c/msys64/mingw64/include)
    link_directories(/c/msys64/mingw64/lib)
    link_libraries(mingw32 fmt::fmt Vulkan::Vulkan OpenAL::OpenAL glfw3 glu32 opengl32 gdi32 m)
endif ()

file(GLOB SRC rendevox-base/*/*.cpp)

add_library(rendevoxlib STATIC ${SRC})

include_directories(rendevox-base)

add_executable(rendevox app/main.cpp)

compile_shader(rendevox
        ENV vulkan
        FORMAT num
        SOURCES
        rendevox-base/vulkanWindow/shaders/shader.vert
        rendevox-base/vulkanWindow/shaders/shader.frag
        )

target_link_libraries(rendevox rendevoxlib)
