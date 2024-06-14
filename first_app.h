//
// Created by - on 6/12/24.
//

#ifndef FIRST_APP_H
#define FIRST_APP_H

#include "brain_window.h"
#include "brain_pipeline.h"
#include "brain_device.h"
#include "brain_swap_chain.h"
#include "brain_model.h"

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
    void loadModels();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    void sierpinski(std::vector<BrnModel::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);

    BrnWindow brnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
    BrnDevice brnDevice{brnWindow};
    BrnSwapChain brnSwapChain{brnDevice, brnWindow.getExtent()};
    std::unique_ptr<BrnPipeline> brnPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<BrnModel> brnModel;
};

} // brn

#endif //FIRST_APP_H
