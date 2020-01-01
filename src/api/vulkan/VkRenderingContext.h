//
// Created by artyomd on 12/31/19.
//

#pragma once

#include "RenderingContext.h"

namespace api {
    class VkRenderingContext : public RenderingContext {
    private:
        VkDevice *device;
        VkPhysicalDevice *physicalDevice;
        VkCommandPool *graphicsPool;
        VkQueue *graphicsQueue;

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    public:
        VkRenderingContext(VkPhysicalDevice *physicalDevice, VkDevice *device,
                           VkCommandPool *graphicsPool, VkQueue *graphicsQueue);

        VkDevice *getDevice() const;

        VertexBuffer *createVertexBuffer(const void *data, unsigned int size) override;

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer &buffer, VkDeviceMemory &bufferMemory);

        void copyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size);

        VkImageView createImageView(VkImage &image, VkFormat format);

        VkCommandBuffer beginSingleTimeCommands(VkCommandPool &commandPool);

        void endSingleTimeCommands(VkQueue &queue, VkCommandPool &pool, VkCommandBuffer &commandBuffer);
    };
}