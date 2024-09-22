#include <SFML/Graphics.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include <chrono>
#include <iostream>

#include "Simulation.hpp"
#include "Profiler.cpp"

#define DEBUG

#ifdef DEBUG
    #define DEBUG_CODE(code) code
#else
    #define DEBUG_CODE(code)
#endif

const int windowWidth = 1000;
const int windowHeight = 800;

const int M = 301;
const int N = 301;
const int vertexArrayWidth = N + 1;
const int vertexArrayHeight = M + 1;

const double cellWidth = windowWidth / (double) N;
const double cellHeight = windowHeight / (double) M;

const double deltaX = 0.1;
const double deltaY = 0.1;
const double deltaT = 0.05;


sf::Color gradientRedBlue(double value) {
    double range = 0.3;
    value = std::clamp(value, -range, range);
    double fraction = sqrt(abs(value) / range);
    int r(0), g(0), b(0);
    if (value > 0) {
        r = static_cast<int> (fraction * 255);
    } else {
        b = static_cast<int> (fraction * 255);
    }
    return sf::Color(r, g, b);
}



sf::Color gradientGrayScale(double value) {
    double range = 0.3;
    value = std::clamp(value, -range, range);
    double fraction = value / (2 * range);
    int r = static_cast<int>(fraction * 255) + 127;
    int g = static_cast<int>(fraction * 255) + 127;
    int b = static_cast<int>(fraction * 255) + 127;
    return sf::Color(r, g, b);
}


sf::VertexArray createVertexArray() {
    sf::VertexArray vertices(sf::Triangles);

    // iterate through each triangle adding the corresponding
    // vertices and their positions

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            vertices.append(sf::Vertex(sf::Vector2f(j * cellWidth, i * cellHeight), sf::Color::Black));
            vertices.append(sf::Vertex(sf::Vector2f(j * cellWidth, (i + 1) * cellHeight), sf::Color::Black));
            vertices.append(sf::Vertex(sf::Vector2f((j + 1) * cellWidth, i * cellHeight), sf::Color::Black));

            vertices.append(sf::Vertex(sf::Vector2f((j + 1) * cellWidth, i * cellHeight), sf::Color::Black));
            vertices.append(sf::Vertex(sf::Vector2f(j * cellWidth, (i + 1) * cellHeight), sf::Color::Black));
            vertices.append(sf::Vertex(sf::Vector2f((j + 1) * cellWidth, (i + 1) * cellHeight), sf::Color::Black));
        }
    }
    return vertices;
}


int convertPixelToIndexX(int x) {
    return static_cast<int>(static_cast<double>(x) / cellWidth);
}


int convertPixelToIndexY(int y) {
    return static_cast<int>(static_cast<double>(y) / cellHeight);
}


int main() {
    DEBUG_CODE(Profiler stepProfiler;Profiler drawProfiler;);
    
    sf::Texture playTexture, pauseTexture;
    sf::Sprite runningSprite;
    sf::Vector2i prevMousePos;
    bool paused = false;
    bool leftIsPressed = false;
    bool rightIsPressed = false;

    if (!playTexture.loadFromFile("../assets/playicon.png") || !pauseTexture.loadFromFile("../assets/pauseicon.png")) {
        std::cout << "Texture Loading Error" << std::endl;
        return 0;
    }
    
    runningSprite.setTexture(playTexture);
    runningSprite.setPosition(sf::Vector2f(0.f, 0.f));
    runningSprite.setColor(sf::Color(255, 255, 255, 255));

    auto start = std::chrono::high_resolution_clock::now();

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "EM Sim", sf::Style::Titlebar | sf::Style::Close);
    Simulation sim(M, N, deltaX, deltaY, deltaT);

    sf::VertexArray vertices = createVertexArray();

    double time = 0.0;

    while (window.isOpen()) {
        if (time >= 100) {
            break;
        }
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    paused = not paused;
                    if (paused) {
                        runningSprite.setTexture(pauseTexture);
                    } else {
                        runningSprite.setTexture(playTexture);
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                leftIsPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                leftIsPressed = false;
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                rightIsPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                rightIsPressed = false;
            }
        }

        if (!paused) {
            DEBUG_CODE(stepProfiler.start(););
            sim.gpuStepElectricField();
            sim.stepRickertSource(time, 0.0);
            sim.gpuStepMagneticField();
            time += deltaT;
            DEBUG_CODE(stepProfiler.stop(););
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int low_x = convertPixelToIndexX(std::min(prevMousePos.x, mousePos.x));
        int high_x = convertPixelToIndexX(std::max(prevMousePos.x, mousePos.x));
        int low_y = convertPixelToIndexY(std::min(prevMousePos.y, mousePos.y));
        int high_y = convertPixelToIndexY(std::max(prevMousePos.y, mousePos.y));

        DEBUG_CODE(drawProfiler.start(););
        if (leftIsPressed) {

            for (int i = low_x; i <= high_x; i++) {
                for (int j = low_y; j <= high_y; j++) {
                    sim.addConductorAt(j, i);
                }
            }
        }

        if (rightIsPressed) {
            for (int i = low_x; i <= high_x; i++) {
                for (int j = low_y; j <= high_y; j++) {
                    sim.removeConductorAt(j, i);
                }
            }
        }

        for (int mm = 0; mm < M; ++mm) {
            for (int nn = 0; nn < N; ++nn) {
                int idx = 6 * (mm * N + nn);
                sf::Color cellColor = gradientRedBlue(sim.E_z.get(mm, nn));
                if (sim.conductorField.get(mm, nn) == 1) {
                    cellColor = sf::Color::Magenta;
                }

                for (int offset = 0; offset < 6; ++offset) {
                    vertices[idx + offset].color = cellColor;
                }
            }
        }

        DEBUG_CODE(drawProfiler.stop(););
        prevMousePos = mousePos;
        window.clear();
        window.draw(vertices);
        window.draw(runningSprite);
        window.display();
    }

    DEBUG_CODE(
        std::cout << "Stepping took: " << stepProfiler.getAverageDuration() << std::endl;
        std::cout << "Drawing took: " << drawProfiler.getAverageDuration() << std::endl;
    );

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time Taken: " << duration << std::endl;

    return 0;
}
