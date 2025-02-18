#include <iostream>

#include <GLFW/glfw3.h>
#include <boxer/boxer.h>

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

        glfwSetWindowOpacity(window, 0.2);

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
    } catch (std::exception e) {
        boxer::show("error", "error");
    }
    return 0;
}