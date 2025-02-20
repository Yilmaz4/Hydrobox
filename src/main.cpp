#include <iostream>
#include <thread>

#include <glad/glad.h>
#include <boxer/boxer.h>

#define SFML_NO_GLU
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>

void renderThread(sf::Window* window) {
    if (!window->setActive(true))
        throw std::runtime_error("Failed to activate SFML window");

    sf::Clock clock;
    clock.start();
    while (window->isOpen()) {   
        float seconds = clock.getElapsedTime().asSeconds();
        glClearColor(seconds - (int)seconds, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        window->display();
    }
}

int main() {
    try {
        sf::Window window{sf::VideoMode({800, 600}), "Hydrobox", sf::Style::Default, sf::State::Windowed, sf::ContextSettings(24, 8, 4, 4, 6)};

        if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
            throw std::runtime_error("Failed to initialize GLAD");
            
        window.setVerticalSyncEnabled(true);
        if (!window.setActive(false))
            throw std::runtime_error("Failed to activate SFML window");

        std::thread thread(&renderThread, &window);

        while (window.isOpen()) {
            while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();
            }
        }
        thread.join();
    } catch (const std::exception& e) {
        boxer::show(e.what(), "Runtime error");
    }
    return 0;
}