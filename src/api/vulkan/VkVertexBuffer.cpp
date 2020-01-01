//
// Created by artyomd on 12/30/19.
//

#include <cstring>
#include "VkVertexBuffer.h"

namespace api {
    VkVertexBuffer::VkVertexBuffer(VkRenderingContext *context, const void *verticesData, VkDeviceSize bufferSize) {
        this->device = context->getDevice();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        context->createBuffer(bufferSize,
                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer,
                              stagingBufferMemory);

        void *data;
        vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, verticesData, (size_t) bufferSize);
        vkUnmapMemory(*device, stagingBufferMemory);

        context->createBuffer(bufferSize,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              buffer,
                              memory);
        context->copyBuffer(stagingBuffer, buffer, bufferSize);
        vkDestroyBuffer(*device, stagingBuffer, nullptr);
        vkFreeMemory(*device, stagingBufferMemory, nullptr);
    }

    VkVertexBuffer::~VkVertexBuffer() {
        vkDestroyBuffer(*device, buffer, nullptr);
        vkFreeMemory(*device, memory, nullptr);
    }
}