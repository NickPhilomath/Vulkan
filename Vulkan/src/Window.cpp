#include "Window.hpp"
#include <stdexcept>

namespace lve {

Window::Window(int w, int h, std::string name) :
	width(w), height(h), windowName(name) {
	initWindow();
}
Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::framebufferResizedCallback(GLFWwindow* window, int width, int height) {
	Window* lveWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	lveWindow->framebufferResized = true;
	lveWindow->width = width;
	lveWindow->height = height;
}

void Window::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
}

void Window::createWindowSurface(VkInstance instanse, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instanse, window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}

}