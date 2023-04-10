#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve {

class Window {
public:
	Window(int w, int h, std::string name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(window); }

	VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
		 
	void createWindowSurface(VkInstance instanse, VkSurfaceKHR* surface);

private:
	static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
	void initWindow();
	int width;
	int height;
	bool framebufferResized = false;
	GLFWwindow* window;
	std::string windowName;
};

}