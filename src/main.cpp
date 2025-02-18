#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <boxer/boxer.h>

GLFWwindow* window;

class Hydrobox {
public:
    GLFWwindow* window;

    Hydrobox() {
        GLFWwindow* window;

        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW.");

        window = glfwCreateWindow(640, 480, "GLFW Demo", NULL, NULL);
        if (!window) {
            throw std::runtime_error("Failed to create GLFW window.");
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD.");
        }

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.0f, 0.f);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwTerminate();
    }
};

int main() {
    try {
        Hydrobox app;
    } catch (const std::exception& e) {
        glfwTerminate();
        boxer::show(e.what(), "Runtime error");
    }
    return 0;
}