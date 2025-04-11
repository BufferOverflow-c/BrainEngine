cmake_minimum_required(VERSION 3.28)
project(BrainEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wno-dev")

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

#set(RAYLIB_VERSION 4.5.0)
#find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
#if (NOT raylib_FOUND) # If there's none, fetch and build raylib
#    include(FetchContent)
#    FetchContent_Declare(
#            raylib
#            DOWNLOAD_EXTRACT_TIMESTAMP OFF
#            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
#    )
#    FetchContent_GetProperties(raylib)
#    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
#        set(FETCHCONTENT_QUIET NO)
#        FetchContent_Populate(raylib)
#        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
#        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
#    endif()
#endif()

add_executable(Brain BrainEngine/src/main.cpp
)

find_package(Vulkan REQUIRED)
#target_link_libraries(Brain ${Vulkan_LIBRARIES})
target_include_directories(Brain PUBLIC ${Vulkan_INCLUDE_DIR})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

#add_subdirectory(BrainEngine/Dependencies/raylib)           #raylib
add_subdirectory(BrainEngine/Dependencies/glfw-3.3.2)       #window opener
add_subdirectory(BrainEngine/Dependencies/glad)             #opengl loader
add_subdirectory(BrainEngine/Dependencies/stb_image)        #loading imaged
add_subdirectory(BrainEngine/Dependencies/stb_truetype)     #loading ttf files
#add_subdirectory(BrainEngine/Dependencies/enet-1.3.17)     #networking
add_subdirectory(BrainEngine/Dependencies/raudio)           #audio
add_subdirectory(BrainEngine/Dependencies/glm)              #math
add_subdirectory(BrainEngine/Dependencies/imgui-docking)    #ui

add_subdirectory(BrainEngine/Dependencies/safeSave)         #saving Data
add_subdirectory(BrainEngine/Dependencies/profilerLib)       #profiling, measuring elapsed time
add_subdirectory(BrainEngine/Dependencies/gl2d)             #2D rendering
add_subdirectory(BrainEngine/Dependencies/glui)             #ui Library, good for making game menus

# add_subdirectory(BrainEngine/include)                       # Brain headers

# MY_SOURCES is defined to be a list of all the source files for my game
# DON'T ADD THE SOURCES BY HAND, they are already added with this macro
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/BrainEngine/src/*.cpp")

#set_property(TARGET "Brain" PROPERTY CXX_STANDARD 17)


#target_compile_definitions("Brain" PUBLIC GLFW_INCLUDE_NONE=1)

if(PRODUCTION_BUILD)
  # setup the ASSETS_PATH macro to be in the root folder of your exe
  target_compile_definitions("Brain" PUBLIC RESOURCES_PATH="./BrainEngine/resources/")

  # remove the option to debug asserts.
  target_compile_definitions("Brain" PUBLIC PRODUCTION_BUILD=1)

else()
  # This is useful to get an ASSETS_PATH in your IDE during development
  target_compile_definitions("Brain" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/BrainEngine/resources/")
  target_compile_definitions("Brain" PUBLIC PRODUCTION_BUILD=0)

endif()

target_sources("Brain" PRIVATE ${MY_SOURCES} )


if(MSVC) # If using the VS compiler...

  target_compile_definitions("Brain" PUBLIC _CRT_SECURE_NO_WARNINGS)

  set_target_properties("Brain" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console

  set_property(TARGET "Brain" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug<$<CONFIG:Debug>:Debug>")
  set_property(TARGET "Brain" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

endif()

target_include_directories("Brain" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/BrainEngine/include/")
target_include_directories("Brain" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/BrainEngine/include/gameLayer/")
target_include_directories("Brain" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/BrainEngine/include/platform/")
target_include_directories("Brain" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/BrainEngine/include/vulkan")

#enet not working yet on linux for some reason
target_link_libraries("Brain" PRIVATE
        #raylib
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
