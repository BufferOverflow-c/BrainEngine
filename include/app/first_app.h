//
// Created by - on 6/12/24.
//

#ifndef FIRST_APP_H
#define FIRST_APP_H

#include "../vulkan/brain_device.h"
#include "../vulkan/brain_game_object.h"
#include "../vulkan/brain_renderer.h"
#include "../vulkan/brain_window.h"

//~ std
#include <memory>
#include <vector>

namespace brn {

class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  //~ Delete copy constructors
  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

private:
  void loadGameObjects();

  BrnWindow brnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
  BrnDevice brnDevice{brnWindow};
  BrnRenderer brnRenderer{brnWindow, brnDevice};
  std::vector<BrnGameObject> gameObjects;
};

} // namespace brn

#endif // FIRST_APP_H
