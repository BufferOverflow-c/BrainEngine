//
// Created by - on 6/12/24.
//

#include "../../include/vulkan/first_app.h"
#include "../../include/vulkan/simple_render_system.h"
#include "vulkan/vulkan_core.h"
//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//~ std
#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace brn {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simplerendersystem{brnDevice,
                                        brnRenderer.getSwapChainRenderPass()};

  while (!brnWindow.shouldClose()) {
    glfwPollEvents();

    if (auto commandBuffer = brnRenderer.beginFrame()) {
      brnRenderer.beginSwapChainRenderPass(commandBuffer);
      simplerendersystem.renderGameObjects(commandBuffer, gameObjects);
      brnRenderer.endSwapChainRenderPass(commandBuffer);
      brnRenderer.endFrame();
    }
  }
  vkDeviceWaitIdle(brnDevice.device());
}

void FirstApp::loadGameObjects() {
  std::vector<BrnModel::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  // sierpinski(vertices, 5, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.f, -0.5f});
  auto brnModel = std::make_shared<BrnModel>(brnDevice, vertices);

  auto triangle = BrnGameObject::createGameObject();
  triangle.model = brnModel;
  triangle.color = {0.1f, 0.8f, 0.1f};
  triangle.transform2d.translation.x = 0.2f;
  triangle.transform2d.scale = {2.f, 0.5f};
  triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

  gameObjects.push_back(std::move(triangle));
}
} // namespace brn
