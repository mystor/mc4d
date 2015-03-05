#include "controls.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Quit the program when the escape key is pressed
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}
