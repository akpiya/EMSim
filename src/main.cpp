#include <SFML/Graphics.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <Simulation.hpp>

const int windowWidth = 1000;
const int windowHeight = 800;
const int M = 101;
const int N = 99;

const double deltaX = 0.1;
const double deltaY = 0.1;
const double deltaT = 0.05;

sf::Color gradientRedBlue(double value) {
    // Assume color values are [-3, 3]
    double range = 10.0;
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



int main()
{
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "SFML works!", sf::Style::Titlebar | sf::Style::Close);
    Simulation sim(M, N, deltaX, deltaY, deltaT);

    double cellWidth = windowWidth / (double) N;
    double cellHeight = windowHeight / (double) M;
    double time = 0.0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        sim.stepMagneticField();
        sim.stepElectricField();
        sim.stepSource(time, 0.0);

        for (int mm = 0; mm < M; ++mm) {
            for (int nn = 0; nn < N; ++nn) {
                sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));

                cell.setPosition(nn * cellWidth, mm * cellHeight);
                cell.setFillColor(gradientRedBlue(sim.E_z[mm][nn]));

                window.draw(cell);
            }
        }
        window.display();
        time += deltaT;
    }

    return 0;
}
