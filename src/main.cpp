#include <SFML/Graphics.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include "Simulation.hpp"

#include <chrono>
#include <iostream>

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

bool paused = false;

sf::Color gradientRedBlue(double value) {
    // Assume color values are [-3, 3]
    double range = 0.5;
    value = std::clamp(value, -range, range);
    double fraction = abs(value) / range;
    int r(0), g(0), b(0);
    if (value > 0) {
        r = static_cast<int> (fraction * 255);
    } else {
        b = static_cast<int> (fraction * 255);
    }
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


int main() {
    sf::Texture playTexture, pauseTexture;
    sf::Sprite runningSprite;

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

    while (window.isOpen())
    {
        if (time >= 100) {
            break;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    paused = not paused;
                    if (paused) {
                        runningSprite.setTexture(pauseTexture);
                    } else {
                        runningSprite.setTexture(playTexture);
                    }
                }
            }
        }

        window.clear();

        if (!paused) {
            sim.stepMagneticField();
            sim.stepElectricField();
            sim.stepRickertSource(time, 0.0);
            time += deltaT;
        }

        for (int mm = 0; mm < M; ++mm) {
            for (int nn = 0; nn < N; ++nn) {
                int idx = 6 * (mm * N + nn);
                sf::Color cellColor = gradientRedBlue(sim.E_z.get(mm, nn));
                for (int offset = 0; offset < 6; ++offset) {
                    vertices[idx + offset].color = cellColor;
                }
            }
        }
        window.draw(vertices);
        window.draw(runningSprite);
        window.display();
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time Taken: " << duration.count() << " microseconds" << std::endl;

    return 0;
}
