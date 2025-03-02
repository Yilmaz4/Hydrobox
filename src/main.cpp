#include <iostream>
#include <thread>
#include <chrono>
#include <random>

#include <glad/glad.h>
#include <boxer/boxer.h>
#include <bmp_read.hpp>
#include <battery/embed.hpp>

#define SFML_NO_GLU
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_theme.hpp>
#include <imgui-SFML.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

struct Particle {
    glm::vec2 pos;
    glm::vec2 vel;
};

GLuint ssbo;
int num_particles = 20;
std::vector<Particle> particles;

void GLAPIENTRY glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    if (type != GL_DEBUG_TYPE_ERROR) return;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", "** GL ERROR **", type, severity, message);
}

void renderThread(sf::RenderWindow& window) {
    if (!window.setActive(true))
        throw std::runtime_error("Failed to activate SFML window");
    if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
        throw std::runtime_error("Failed to initialize GLAD");

    sf::Vector2i windowPos = window.getPosition();
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u screenSize = sf::VideoMode::getDesktopMode().size;

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glMessageCallback, nullptr);

    b::EmbedInternal::EmbeddedFile embed;
    const char* content;
    int length;

    auto check_for_errors = [](GLuint shader) {
        int success;
        char infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            boxer::show(infoLog, "Fatal shader compilation error", boxer::Style::Error);
            throw;
        }
    };

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    embed = b::embed<"shaders/vertex.glsl">();
    content = embed.data();
    length = embed.length();
    glShaderSource(vertexShader, 1, &content, NULL);
    glCompileShader(vertexShader);
    check_for_errors(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    embed = b::embed<"shaders/fragment.glsl">();
    content = embed.data();
    length = embed.length();
    glShaderSource(fragShader, 1, &content, NULL);
    glCompileShader(fragShader);
    check_for_errors(fragShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glUseProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    embed = b::embed<"shaders/compute.glsl">();
    content = embed.data();
    length = embed.length();
    glShaderSource(computeShader, 1, &content, NULL);
    glCompileShader(computeShader);
    check_for_errors(computeShader);

    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    glDeleteShader(computeShader);

    glShaderStorageBlockBinding(program, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "ssbo"), 0);
    glShaderStorageBlockBinding(computeProgram, glGetProgramResourceIndex(computeProgram, GL_SHADER_STORAGE_BLOCK, "ssbo"), 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    while (windowPos.x <= 0 || windowPos.y <= 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        windowPos = window.getPosition();
        windowSize = window.getSize();
    }
    for (int i = 0; i < num_particles; i++) {
        Particle p;
        
        p.pos = { (float)windowPos.x / windowSize.x + dist(e2), (float)windowPos.y / windowSize.y + dist(e2) };
        p.vel = { 0.f, 0.f };
        particles.push_back(p);
    }

    glBufferData(GL_SHADER_STORAGE_BUFFER, num_particles * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    sf::Clock clock;
    clock.start();
    sf::Time prevTime;

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glPointSize(10.f);

    sf::Vector2i prevWindowPos = windowPos;

    while (window.isOpen()) {
        sf::Time elapsed = clock.getElapsedTime();
        sf::Time sfdt = elapsed - prevTime;
        float dt = sfdt.asSeconds();
        prevTime = elapsed;
        ImGui::SFML::Update(window, sfdt);

        windowPos = window.getPosition();
        windowSize = window.getSize();
        vec2 corner{ (float)windowPos.x / windowSize.x, (float)windowPos.y / windowSize.y };
        vec2 prevCorner{ (float)prevWindowPos.x / windowSize.x, (float)prevWindowPos.y / windowSize.y };
        vec2 windowVelocity = (corner - prevCorner) / dt;
        prevWindowPos = windowPos;

        glUseProgram(computeProgram);
        glUniform1f(glGetUniformLocation(computeProgram, "dt"), dt);
        glUniform2f(glGetUniformLocation(computeProgram, "windowVelocity"), windowVelocity.x, windowVelocity.y);
        glUniform2f(glGetUniformLocation(computeProgram, "corner"), corner.x, corner.y);
        glDispatchCompute(num_particles, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        glUseProgram(program);
        glUniform2f(glGetUniformLocation(program, "windowSize"), windowSize.x, windowSize.y);
        glUniform2f(glGetUniformLocation(program, "windowPos"), windowPos.x, windowPos.y);
        glUniform2f(glGetUniformLocation(program, "windowVelocity"), windowVelocity.x, windowVelocity.y);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_POINTS, 0, num_particles);
        ImGui::SFML::Render();
        window.display();
    }
}

int main() {
    try {
        sf::RenderWindow window{sf::VideoMode({ 1200, 800 }), "Hydrobox",
            sf::Style::Default, sf::State::Windowed, sf::ContextSettings(24, 8, 4, 4, 6)};
        window.setVerticalSyncEnabled(true);
        auto icon = b::embed<"assets/icon.bmp">();
        uint8_t pixels[48 * 48 * 4];
        parseBMP(reinterpret_cast<const uint8_t*>(icon.data()), icon.size(), pixels);
        window.setIcon({ 48, 48 }, pixels);

        if (!ImGui::SFML::Init(window, false))
            throw std::runtime_error("Failed to initialize ImGui");
        if (!window.setActive(false))
            throw std::runtime_error("Failed to activate SFML window");
        
        IMGUI_CHECKVERSION();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        auto font = b::embed<"assets/consola.ttf">();
        io.Fonts->Clear();
        io.IniFilename = NULL;
        io.LogFilename = NULL;
        ImFont* imfont = io.Fonts->AddFontFromMemoryTTF((void*)font.data(), font.size(), 11.f, nullptr);
        IM_ASSERT(imfont != NULL);
        if (!ImGui::SFML::UpdateFontTexture())
            throw std::runtime_error("Failed to update font texture in ImGui");
        ImGui::LoadTheme();

        std::thread thread(&renderThread, std::ref(window));

        while (window.isOpen()) {
            while (const auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>())
                    window.close();
                else if (const auto re = event->getIf<sf::Event::Resized>()) {
                    sf::Vector2f newsize = { static_cast<float>(re->size.x), static_cast<float>(re->size.y) };
                    window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, newsize)));
                }
                ImGui::SFML::ProcessEvent(window, event.value());
            }
        }
        thread.join();
    } catch (const std::runtime_error& e) {
        boxer::show(e.what(), "Runtime error", boxer::Style::Error);
        return 1;
    }
    return 0;
}