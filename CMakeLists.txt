cmake_minimum_required(VERSION 4.2)
project(FirstProject)

include(FetchContent)
FETCHCONTENT_DECLARE(GLFW GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_MakeAvailable(GLFW)

find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    include_directories(${Vulkan_INCLUDE_DIRS})
    link_directories(${Vulkan_LIBRARY_DIRS})
endif()


set(CMAKE_CXX_STANDARD 26)

add_executable(FirstProject main.cpp
        src/HelloTriangleApplication.cpp
        esqueleto.cpp
        HelloTriangleApplication.cpp
)

target_link_libraries(FirstProject
        glfw
        ${Vulkan_LIBRARIES}
)
