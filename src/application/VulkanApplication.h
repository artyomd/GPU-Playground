//
// Created by artyomd on 12/7/19.
//

#pragma once

#define GLFW_INCLUDE_VULKAN

#include "GLFWApplication.h"
#include <vulkan/vulkan.h>

namespace application {
    class VulkanApplication : public GLFWApplication {
    private:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        const int MAX_FRAMES_IN_FLIGHT = 2;

        VkInstance vulkanInstance;
        VkDebugUtilsMessengerEXT debugMessenger;

        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;

        VkRenderPass renderPass;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkSwapchainKHR swapChain;
        VkExtent2D swapChainExtent;
        VkFormat swapChainImageFormat;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFrameBuffers;

        VkDescriptorPool descriptorPool;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        bool framebufferResized = false;
        int currentFrame = 0;
        uint32_t currentImage = 0;

        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() {
                return graphicsFamily.has_value() &&
                       presentFamily.has_value();
            }
        };

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities{};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        void createInstance();

        void rereateSwapChain();

        bool checkValidationLayerSupport();

        std::vector<const char *> getRequiredExtensions();

        void setupDebugMessenger();

        void createSurface();

        bool isDeviceSuitable(VkPhysicalDevice device);

        void pickPhysicalDevice();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        void createLogicalDevice();

        void createDescriptorPool();

        void createCommandPool();

        void createSwapChain();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createImageViews();

        ////maybe this should be an api method
        VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        //// end maybe api methods

        void createRenderPass();

        void createFramebuffers();

        void createCommandBuffers();

        void createSyncObjects();

        void cleanupSwapChain();

    protected:
        void setupWindowHints() final;

        void onWindowSizeChanged() final;

        void initImGui() final;

        bool prepareFrame() final;

        void createImGuiFrame() final;

        void renderImGui() final;

        void drawFrame() final;

        void destroyImGui() final;

        void prepareForShutdown() final;

    public:
        void initContext() final;

        void destroyContext() override;
    };
}
