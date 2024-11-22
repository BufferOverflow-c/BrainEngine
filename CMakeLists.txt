include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)

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

#! ! ! ! ! ! !
#set this to true to ship the game!
#basically this will change the RESOURCES_PATH to be the local path
#! ! ! ! ! ! !
#DELETE THE OUT FOLDER AFTER CHANGING THIS BECAUSE VISUAL STUDIO DOESN'T SEEM TO RECOGNIZE THIS CHANGE AND REBUILD!
option(PRODUCTION_BUILD "Make this a production build" OFF)
#DELETE THE OUT FOLDER AFTER CHANGING THIS BECAUSE VISUAL STUDIO DOESN'T SEEM TO RECOGNIZE THIS CHANGE AND REBUILD!

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) #link time optimization

if(MSVC)
    add_compile_options(/arch:AVX2) #make sure SIMD optimizations take place
endif()

#add_executable(Brain BrainEngine/src/main.cpp)

if(NOT DEFINED VULKAN_SDK_PATH)
  find_package(Vulkan REQUIRED)
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
else()
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/include") # 1.1 Make sure this include path is correct
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/lib") # 1.2 Make sure lib path is correct
  set(Vulkan_FOUND "True")
endif()
if(NOT Vulkan_FOUND)
  nessage(FATAL_ERROR "Could not find Vulkan Library")
else()
  message(STATUS "Using Vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

if(NOT DEFINED GLFW_PATH)
  find_package(glfw3 REQUIRED)
  message(STATUS "Found GLFW3")
else()
  message(STATUS "Using GLFW path specified in .env")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
  if(MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-universal") # 2.1 Update lib-vc2019 to use same version as your visual studio
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${GLFW_PATH}/lib-universal") # 2.1 make sure matches glfw mingw subdirectory
  endif()
endif()
# maybe need glfw lib definition here

if(NOT TINYOBJ_PATH)
  message(STATUS "TINYOBJ_PATH not specified in .env.cmake, using Dependencies/tinyobjloader")
  set(TINYOBJ_PATH Dependencies/tinyobjloader)
endif()

if(NOT DEFINED GLM_PATH)
  find_package(glm REQUIRED)
  message(STATUS "Found GLM")
else()
  message(STATUS "Could not find GLM, using Dependencies/glm")
  set(GLM_PATH Dependencies/glm)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(Dependencies/glad)             #opengl loader
add_subdirectory(Dependencies/stb_image)        #loading imaged
add_subdirectory(Dependencies/stb_truetype)     #loading ttf files
#add_subdirectory(Dependencies/enet-1.3.17)     #networking
add_subdirectory(Dependencies/raudio)           #audio
add_subdirectory(Dependencies/glm)              #math
add_subdirectory(Dependencies/imgui-docking)    #ui

add_subdirectory(Dependencies/safeSave)         #saving Data
add_subdirectory(Dependencies/profilerLib)       #profiling, measuring elapsed time
add_subdirectory(Dependencies/gl2d)             #2D rendering
add_subdirectory(Dependencies/glui)             #ui Library, good for making game menus

# add_subdirectory(BrainEngine/include)                       # Brain headers

# MY_SOURCES is defined to be a list of all the source files for my game
# DON'T ADD THE SOURCES BY HAND, they are already added with this macro
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${MY_SOURCES})

if(PRODUCTION_BUILD)
    # setup the ASSETS_PATH macro to be in the root folder of your exe
    target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCES_PATH="./resources/")

    # remove the option to debug asserts.
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1)

else()
    # This is useful to get an ASSETS_PATH in your IDE during development
    target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=0)

endif()

target_sources(${PROJECT_NAME} PRIVATE ${MY_SOURCES})


if(MSVC) # If using the VS compiler...

  target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console

    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug<$<CONFIG:Debug>:Debug>")
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

endif()

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/app")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/gameLayer/")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/platform/")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/systems")

#enet not working yet on linux for some reason
target_link_libraries(${PROJECT_NAME} PRIVATE
        glm
        glfw
        glad
        stb_image
        stb_truetype
        gl2d
        raudio
        imgui
        safeSave
        profilerLib
        glui
        Vulkan::Vulkan
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
)
 
############## Build SHADERS #######################
 
# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)
 
# get all .vert and .frag files in shaders directory
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
endforeach(GLSL)
 
add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
