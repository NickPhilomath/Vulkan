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

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};

    LveCamera camera{};
    float aspect = lveRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 40.f);
    KeyboardMovementController cameraController{};
    
    auto viewerObject = LveGameObject::createGameObject();
    viewerObject.transform.translation = { 0.f, -2.f, -5.f };

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!lveWindow.shouldClose()) {
        // get events
        glfwPollEvents();
        // delta time 
        auto newTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;
        // update
        cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), deltaTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
        // render
        if (auto commandBuffer = lveRenderer.beginFrame()) {
            lveRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
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
    gameObjects.push_back(std::move(player));

    auto plane = LveGameObject::createGameObject();
    plane.model = LveModel::createModelFromFile(lveDevice, "models/plane.obj");
    plane.transform.translation = { 0.0f, 0.0f, 0.0f };
    plane.transform.rotation = { 0.0f, 0.0f, 0.0f };
    plane.color = rgbToTheroOne(73, 143, 100);
    gameObjects.push_back(std::move(plane));
}

}  // namespace lve
