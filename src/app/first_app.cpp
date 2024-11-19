//
// Created by - on 6/12/24.
//

// app
#include "../../include/app/first_app.h"

// engine
#include "../../include/app/keyboard_movement_controller.h"
#include "../../include/vulkan/brain_buffer.h"
#include "../../include/vulkan/brain_camera.h"
#include "../../include/vulkan/simple_render_system.h"

//~ libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vulkan/vulkan_core.h>

//~ std
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace brn {

struct GlobalUbo {
  glm::mat4 projectionView{1.f};
  glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
};

FirstApp::FirstApp() {
  globalPool = BrnDescriptorPool::Builder(brnDevice)
                   .setMaxSets(BrnSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                BrnSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .build();
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
  std::vector<std::unique_ptr<BrnBuffer>> uboBuffers(
      BrnSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<BrnBuffer>(
        brnDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  auto globalSetLayout = BrnDescriptorSetLayout::Builder(brnDevice)
                             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         VK_SHADER_STAGE_VERTEX_BIT)
                             .build();

  std::vector<VkDescriptorSet> globalDescriptorSets(
      BrnSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    BrnDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
  }

  SimpleRenderSystem simpleRenderSystem{
      brnDevice, brnRenderer.getSwapChainRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};

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
      int frameIndex = brnRenderer.getFrameIndex();
      FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera,
                          globalDescriptorSets[frameIndex]};

      // update
      GlobalUbo ubo{};
      ubo.projectionView = camera.getProjection() * camera.getView();
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // render
      brnRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
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
