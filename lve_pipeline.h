//
// Created by - on 6/12/24.
//

#ifndef LVE_PIPELINE_H
#define LVE_PIPELINE_H

#include "lve_device.h"

//~ std
#include <string>
#include <vector>

namespace lve {
    struct PiplineConfigInfo {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

class LvePipeline {
public:
    LvePipeline(LveDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PiplineConfigInfo& configInfo);
    ~LvePipeline();

    //~ Delete Copy Constructors
    LvePipeline(const LvePipeline&) = delete;
    void operator=(const LvePipeline&) = delete;

    static PiplineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

private:
    static std::vector<char> readFile(const std::string& filePath);

    void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PiplineConfigInfo& configInfo);

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    LveDevice& lveDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};
} // lve

#endif //LVE_PIPELINE_H
