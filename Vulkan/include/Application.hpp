#pragma once

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
		void drawFrame();


		Window lveWindow{WIDTH, HEIGHT, "hello nulkan!"};
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };
		std::unique_ptr<PipeLine> lvePipeline;
		//PipeLine lvePipeline{lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", PipeLine::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<LveModel> lveModel;
	};
}