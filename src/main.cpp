#include "wgpu_init.h"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_wgpu.h>
#include <imgui.h>
#include <iostream>

int main()
{

  // WGPUDevice device = requestWGPUDevice();
  // ImGui_ImplWGPU_Init(device, 3, WGPUTextureFormat_RGBA8Unorm);
  if (!glfwInit()) {
    std::cerr << "Could not initialize GLFW!" << std::endl;
    return 1;
  }

  GLFWwindow *window
      = glfwCreateWindow(640, 480, "LearnWebGPU", nullptr, nullptr);

  if (!window) {
    std::cerr << "GLFW could not open window!" << std::endl;
    glfwTerminate();
    return 1;
  }

  while (!glfwWindowShouldClose(window)) {
    // Check whether the user clicked on the close button (and any other
    // mouse/key event, which we don't use so far)
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
}
