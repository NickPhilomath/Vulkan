#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Window.hpp"
#include "Pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"
#include <memory>
#include <vector>

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
	void loadModels();
	void createPipeLineLayout();
	void createPipeLine();
	void createCommandBuffers();
	void freeCommandBuffers();
	void drawFrame();
	void recreateSwapChain();
	void recordCommandBuffer(int imageIndex);


	Window lveWindow{WIDTH, HEIGHT, "hello nulkan!"};
	LveDevice lveDevice{ lveWindow };
	std::unique_ptr<LveSwapChain> lveSwapChain;
	std::unique_ptr<PipeLine> lvePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
	std::unique_ptr<LveModel> lveModel;
};

}