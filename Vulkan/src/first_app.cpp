#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

#include <iostream>

glm::vec3 rgbToTheroOne(int r, int g, int b) {
    return glm::vec3{ r / 255.f, g / 255.f, b / 255.f };
}

namespace lve {

struct GlobalUbo {
    alignas(16) glm::mat4 projection{ 1.f };
    alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
};

FirstApp::FirstApp() { 
    globalPool = LveDescriptorPool::Builder(lveDevice)
        .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();
    loadGameObjects(); 
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
    std::vector<std::unique_ptr<LveBuffer>> ubobuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < ubobuffers.size(); i++) {
        ubobuffers[i] = std::make_unique<LveBuffer>(
            lveDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT //  | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        ubobuffers[i]->map();
    }

    auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = ubobuffers[i]->descriptorInfo();
        LveDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

    LveCamera camera{};
    float aspect = lveRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 40.f);
    camera.transform.translation = { 0.f, -2.f, -15.f };

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!lveWindow.shouldClose()) {
        // delta time 
        auto newTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        // get events
        glfwPollEvents();
        // update
        camera.update(lveWindow.getGLFWwindow(), deltaTime);
        gameObjects[0].update(deltaTime);
        // render
        if (auto commandBuffer = lveRenderer.beginFrame()) {
            int frameIndex = lveRenderer.getFrameIndex();
            FrameInfo frameInfo{ frameIndex, deltaTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};
            // update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection() * camera.getView();
            ubobuffers[frameIndex]->writeToBuffer(&ubo);
            ubobuffers[frameIndex]->flush();
            // render
            lveRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
            lveRenderer.endSwapChainRenderPass(commandBuffer);
            lveRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
    auto flatVase = LveGameObject::createGameObject();
    flatVase.model = lveModel;
    flatVase.transform.translation = {-.5f, .5f, 2.5f};
    flatVase.transform.scale = {3.f, 1.5f, 3.f};
    //gameObjects.push_back(std::move(flatVase));

    lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
    auto smoothVase = LveGameObject::createGameObject();
    smoothVase.model = lveModel;
    smoothVase.transform.translation = {.5f, .5f, 2.5f};
    smoothVase.transform.scale = {3.f, 1.5f, 3.f};
    //gameObjects.push_back(std::move(smoothVase));

    auto tiger_1 = LveGameObject::createGameObject();
    tiger_1.model = LveModel::createModelFromFile(lveDevice, "models/tanks/Tiger_I.obj");
    tiger_1.transform.translation = { .0f, 1.0f, 3.0f };
    tiger_1.transform.rotation = { 0.0f, 0.0f, 0.0f };
    tiger_1.transform.scale = { 0.4f, 0.4f, 0.4f };
    //gameObjects.push_back(std::move(tiger_1));

    auto player = LveGameObject::createGameObject();
    player.model = LveModel::createModelFromFile(lveDevice, "models/player.obj");
    player.transform.translation = { 0.0f, 0.0f, 0.0f };
    player.transform.rotation = { 0.0f, 0.0f, 0.0f };
    player.transform.scale = { 0.5f, 0.5f, 0.5f };
    player.color = rgbToTheroOne(0, 162, 255);
    player.gravity = 0.25f;
    player.speed = -0.20f;
    gameObjects.push_back(std::move(player));

    auto cube = LveGameObject::createGameObject();
    cube.model = LveModel::createModelFromFile(lveDevice, "models/colored_cube.obj");
    cube.transform.translation = { 2.0f, -1.0f, 0.0f };
    cube.transform.rotation = { 0.0f, 0.0f, 0.0f };
    cube.transform.scale = { 0.5f, 0.5f, 0.5f };
    //gameObjects.push_back(std::move(cube));

    auto viking_room = LveGameObject::createGameObject();
    viking_room.model = LveModel::createModelFromFile(lveDevice, "models/viking_room.obj");
    viking_room.transform.translation = { 3.0f, -1.0f, 0.0f };
    viking_room.transform.rotation = { 3.14f / 2, 0.0f, 3.14f };
    viking_room.color = rgbToTheroOne(0, 162, 255);
    gameObjects.push_back(std::move(viking_room));

    auto plane = LveGameObject::createGameObject();
    plane.model = LveModel::createModelFromFile(lveDevice, "models/plane.obj");
    plane.transform.translation = { 0.0f, 0.0f, 0.0f };
    plane.transform.rotation = { 0.0f, 0.0f, 0.0f };
    plane.color = rgbToTheroOne(73, 143, 100);
    gameObjects.push_back(std::move(plane));
}

}  // namespace lve
