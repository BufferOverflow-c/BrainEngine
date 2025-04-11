include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(NAME BrainEngine)

message(STATUS "using ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
  if (NOT MINGW_PATH)
    message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
  endif()
  set(USE_MINGW "True")
  set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
  set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)
endif()

project(${NAME} VERSION 0.0.1)

# Set options for production build
option(PRODUCTION_BUILD "Make this a production build" OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # Link-time optimization

if(MSVC)
    add_compile_options(/arch:AVX2) # Ensure SIMD optimizations take place
endif()

# Find Vulkan
find_package(Vulkan REQUIRED)

# GLFW setup
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW Build Documentation")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW Build Examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW Build Tests")

# Add subdirectories for dependencies
add_subdirectory(Dependencies/glfw)
add_subdirectory(Dependencies/glm)
add_subdirectory(Dependencies/glad)
add_subdirectory(Dependencies/stb_image)
add_subdirectory(Dependencies/stb_truetype)
add_subdirectory(Dependencies/raudio)
add_subdirectory(Dependencies/gl2d)
add_subdirectory(Dependencies/imgui-docking)
add_subdirectory(Dependencies/safeSave)
add_subdirectory(Dependencies/profilerLib)
add_subdirectory(Dependencies/glui)

# Collect all source files
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Create executable target
add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Include directories (now after the target creation)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_INCLUDE_DIRS}                                   # Vulkan headers
    ${CMAKE_SOURCE_DIR}/include                                # Local headers
    ${CMAKE_SOURCE_DIR}/include/app
    ${CMAKE_SOURCE_DIR}/include/gameLayer
    ${CMAKE_SOURCE_DIR}/include/platform
    ${CMAKE_SOURCE_DIR}/include/vulkan
    ${CMAKE_SOURCE_DIR}/include/vulkan/systems
    ${CMAKE_SOURCE_DIR}/Dependencies/glfw/include              # GLFW headers
    ${CMAKE_SOURCE_DIR}/Dependencies/glm                       # GLM headers
)

# Set compile definitions based on build type
if(PRODUCTION_BUILD)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCES_PATH="./resources/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=0)
endif()

# Link libraries and dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan                                               # Link Vulkan
    glfw                                                         # Link GLFW
    glm                                                          # Link GLM
    glad                                                         # Link glad
    stb_image                                                    # Link stb_image
    stb_truetype                                                 # Link stb_truetype
    gl2d                                                         # Link gl2d
    raudio                                                       # Link raudio
    imgui                                                        # Link imgui
    safeSave                                                     # Link safeSave
    profilerLib                                                  # Link profilerLib
    glui                                                         # Link glui
    "-framework IOKit"
    "-framework Cocoa"
    "-framework OpenGL"
)

# Shader Compilation
find_program(GLSL_VALIDATOR glslangValidator HINTS
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
  /usr/bin
  /usr/local/bin
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/
  $ENV{VULKAN_SDK}/Bin32/
)

file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})
