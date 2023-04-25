#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

#include "Window.hpp"
#include "Pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"

namespace lve {

class Application {
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	Application();
	~Application();

	Application(const Window&) = delete;
	Application& operator=(const Application&) = delete;

	void run();

private:
	void loadGameObjects();
	void createPipeLineLayout();
	void createPipeLine();
	void createCommandBuffers();
	void freeCommandBuffers();
	void drawFrame();
	void recreateSwapChain();
	void recordCommandBuffer(int imageIndex);
	void renderGameObjects(VkCommandBuffer commandBuffer);


	Window lveWindow{WIDTH, HEIGHT, "hello pain in the ass!"};
	LveDevice lveDevice{ lveWindow };
	std::unique_ptr<LveSwapChain> lveSwapChain;
	std::unique_ptr<PipeLine> lvePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<LveGameObject> gameObjects;
};

}