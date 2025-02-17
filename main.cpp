#include <iostream>
#include <GLFW/glfw3.h>

GLFWwindow* window;

class Hydrobox {
public:
    GLFWwindow* window;

    Hydrobox() {
        GLFWwindow* window;

        if (!glfwInit()) throw std::exception();

        window = glfwCreateWindow(640, 480, "GLFW Demo", NULL, NULL);
        if (!window) {
            glfwTerminate();
            throw std::exception();
        }

        glfwMakeContextCurrent(window);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(1.0f, 0.0f, 0.0f, 1.f);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glfwTerminate();
    }
};

int main() {
    try {
        Hydrobox app;
    } catch (std::exception e) {
        return -1;
    }
    return 0;
}