//
// Created by artyomd on 12/30/19.
//

#pragma once

#include <vulkan/vulkan.h>
#include <VertexBuffer.h>
#include "VkRenderingContext.h"

namespace api {
    class VkVertexBuffer : public VertexBuffer {
    private:
        VkDevice *device;
        VkBuffer buffer{};
        VkDeviceMemory memory{};
    public:
        VkVertexBuffer(VkRenderingContext *context, const void *data, VkDeviceSize bufferSize);

        ~VkVertexBuffer();
    };
}