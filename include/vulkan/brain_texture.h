#ifndef BRAIN_TEXTURE_H
#define BRAIN_TEXTURE_H

#include "brain_device.h"

#include <string>

namespace brn{
class BrnTexture {
public:
    BrnTexture(BrnDevice &device, const std::string &filepath);
    ~BrnTexture();

    BrnTexture(const BrnTexture &) = delete;
    BrnTexture &operator=(const BrnTexture &) = delete;
    BrnTexture(BrnTexture &&) = delete;
    BrnTexture &operator=(BrnTexture &&) = delete;

    VkSampler getSampler() { return sampler; }
    VkImageView getImageView() { return imageView; }
    VkImageLayout getImageLayout() { return imageLayout; }
private:
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void generateMipmaps();

    int width;
    int height;
    int mipLevels;

    BrnDevice &brnDevice;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler sampler;
    VkFormat imageFormat;
    VkImageLayout imageLayout;

};
}

#endif  // BRAIN_TEXTURE_H
