//
// Created by - on 6/12/24.
//

#include "../../include/app/first_app.h"
#include "../../include/app/keyboard_movement_controller.h"
#include "../../include/vulkan/brain_camera.h"
#include "../../include/vulkan/simple_render_system.h"

#include "vulkan/vulkan_core.h"

//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//~ std
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace brn {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simplerendersystem{brnDevice,
                                        brnRenderer.getSwapChainRenderPass()};

  BrnCamera camera{};
  camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

  auto viewerObject = BrnGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!brnWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(brnWindow.getGLFWwindow(), frameTime,
                                   viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = brnRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10);

    if (auto commandBuffer = brnRenderer.beginFrame()) {
      brnRenderer.beginSwapChainRenderPass(commandBuffer);
      simplerendersystem.renderGameObjects(commandBuffer, gameObjects, camera);
      brnRenderer.endSwapChainRenderPass(commandBuffer);
      brnRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(brnDevice.device());
}

void FirstApp::loadGameObjects() {
  std::shared_ptr<BrnModel> brnModel =
      BrnModel::createModelFromFile(brnDevice, "../../models/flat_vase.obj");

  auto gameObj = BrnGameObject::createGameObject();
  gameObj.model = brnModel;
  gameObj.transform.translation = {0.f, 0.5f, 2.5f};
  gameObj.transform.scale = glm::vec3{3.f};

  gameObjects.push_back(std::move(gameObj));
}
} // namespace brn
