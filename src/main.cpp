#include <iostream>
#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <boxer/boxer.h>

#define SFML_NO_GLU
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <imgui_theme.hpp>

#include "battery/embed.hpp"

ImFont* imfont;

void renderThread(sf::RenderWindow& window) {
    if (!window.setActive(true))
        throw std::runtime_error("Failed to activate SFML window");

    sf::Clock clock;
    clock.start();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    sf::Time prevTime;

    while (window.isOpen()) {
        sf::Time elapsed = clock.getElapsedTime();
        sf::Time dt = elapsed - prevTime;
        prevTime = elapsed;
        ImGui::SFML::Update(window, dt);

        //ImGui::PushFont(imfont);

        ImGui::ShowDemoWindow();

        //ImGui::PopFont();

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::SFML::Render();
        window.display();
    }
}

int main() {
    try {
        sf::RenderWindow window{sf::VideoMode({800, 600}), "Hydrobox", sf::Style::Default, sf::State::Windowed, sf::ContextSettings(24, 8, 4, 4, 6)};
        window.setVerticalSyncEnabled(true);

        if (!ImGui::SFML::Init(window, false))
            throw std::runtime_error("Failed to initialize ImGui");
        if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
            throw std::runtime_error("Failed to initialize GLAD");
        if (!window.setActive(false))
            throw std::runtime_error("Failed to activate SFML window");

        
        IMGUI_CHECKVERSION();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        auto font = b::embed<"assets/consola.ttf">();
        io.Fonts->Clear();
        imfont = io.Fonts->AddFontFromMemoryTTF((void*)font.data(), font.size(), 11.f, nullptr);
        IM_ASSERT(imfont != NULL);
        if (!ImGui::SFML::UpdateFontTexture())
            throw std::runtime_error("Failed to update font texture in ImGui");
        ImGui::LoadTheme();

        std::thread thread(&renderThread, std::ref(window));

        while (window.isOpen()) {
            while (const auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) window.close();
                ImGui::SFML::ProcessEvent(window, event.value());
            }
        }
        thread.join();
    } catch (const std::exception& e) {
        boxer::show(e.what(), "Runtime error");
    }
    return 0;
}