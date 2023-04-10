#include "Application.hpp"
#include <iostream>
#include <stdexcept>
#include <array>

namespace lve {

struct SimplePushConstantData {
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Application::Application() {
	loadModels();
	createPipeLineLayout();
	recreateSwapChain();
	createCommandBuffers();
}

Application::~Application() {
	vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
}

void Application::run() {
	while (!lveWindow.shouldClose())
	{
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(lveDevice.device());
}

void Application::loadModels() {
	std::vector<LveModel::Vertex> vertices {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	lveModel = std::make_unique<LveModel>(lveDevice, vertices);
}

void Application::createPipeLineLayout() {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout");
	}
}

void Application::createPipeLine() {
	assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipline layout");

	PipelineConfigInfo pipelineConfig{};
	PipeLine::defaultPipelineConfigInfo(pipelineConfig);
	//auto pipelineConfig = PipeLine::defaultPipelineConfigInfo(lveSwapChain->width(), lveSwapChain->height());
	pipelineConfig.renderPass = lveSwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;
	lvePipeline = std::make_unique<PipeLine>(
		lveDevice,
		"shaders/simple_shader.vert.spv",
		"shaders/simple_shader.frag.spv",
		pipelineConfig
	);
}

void Application::createCommandBuffers() {
	commandBuffers.resize(lveSwapChain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = lveDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}
	/*
	for (int i = 0; i < commandBuffers.size(); i++) {
		
	}
	*/
}

void Application::freeCommandBuffers() {
	vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	commandBuffers.clear();
}

void Application::drawFrame() {
	uint32_t imageIndex;
	auto result = lveSwapChain->acquireNextImage(&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image");
	}

	recordCommandBuffer(imageIndex);

	result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
		lveWindow.resetWindowResizedFlag();
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image");
	}
}

void Application::recreateSwapChain() {
	auto extend = lveWindow.getExtent();
	while (extend.width == 0 || extend.height == 0) {
		extend = lveWindow.getExtent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(lveDevice.device());

	if (lveSwapChain == nullptr) {
		lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extend);
	}
	else {
		lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extend, std::move(lveSwapChain));
		if (lveSwapChain->imageCount() != commandBuffers.size()) {
			freeCommandBuffers();
			createCommandBuffers();
		}
	}

	createPipeLine();
}

void Application::recordCommandBuffer(int imageIndex) {
	static int frame = 0;
	frame = (frame + 1) % 100;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = lveSwapChain->getRenderPass();
	renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };
	clearValues[1].depthStencil = { 0.1f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, lveSwapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

	lvePipeline->bind(commandBuffers[imageIndex]);
	lveModel->bind(commandBuffers[imageIndex]);

	for (int j = 0; j < 4; j++) {
		SimplePushConstantData push{};
		push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
		push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

		vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

		lveModel->draw(commandBuffers[imageIndex]);
	}



	vkCmdEndRenderPass(commandBuffers[imageIndex]);

	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer");
	}
}

}