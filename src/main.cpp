#include "RNN.hpp"
#include "RNNRenderer.hpp"
#include "RNNMaker.hpp"
#include "Creature.hpp"
#include "World.hpp"
#include "Globals.hpp"
#include "Clock.hpp"

#include <SFML/Graphics.hpp>


int main(void) {
    World world(globals::worldW, globals::worldH);

    sf::RenderWindow window(sf::VideoMode(globals::viewPortW, globals::viewPortH), "Evolution",
        sf::Style::Fullscreen, sf::ContextSettings(0, 0, 4));
    window.setFramerateLimit(30);

    int64_t zoomLevel = 10;
    double zoomFactor = pow(1.2, zoomLevel) * 0.16150558288984572135;
    sf::View view;
    view.reset(sf::FloatRect(0, 0, globals::viewPortW, globals::viewPortH));
    view.setSize(globals::viewPortW*zoomFactor, globals::viewPortH*zoomFactor);
    view.setCenter(globals::worldW/2, globals::worldH/2);

    window.setView(view);

    sf::Vector2f cursorPos;

    uint64_t t = 0;
    uint64_t speed = 1;
    bool autoSpeed = false;

    Clock c;
    double tickTime;

    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Num1:
                    speed = 1;
                    autoSpeed = false;
                    break;
                case sf::Keyboard::Num2:
                    speed = 10;
                    autoSpeed = false;
                    break;
                case sf::Keyboard::Num3:
                    speed = 100;
                    autoSpeed = false;
                    break;
                case sf::Keyboard::Num4:
                    speed = 1;
                    autoSpeed = true;
                    break;
                default:
                    break;
                }
                break;
            case sf::Event::MouseMoved:
                cursorPos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                break;
            case sf::Event::MouseWheelMoved:
                {
                    sf::Vector2f viewCenterToCursorScreen = cursorPos - sf::Vector2f(globals::viewPortW*0.5f, globals::viewPortH*0.5f);
                    sf::Vector2f cursorWorld = view.getCenter() + (viewCenterToCursorScreen * (float)zoomFactor);
                    sf::Vector2f viewCenterToCursorWorld = (cursorWorld - view.getCenter()) / (float)zoomFactor;

                    zoomLevel -= event.mouseWheel.delta;
                    if (zoomLevel <= 0) {
                        zoomLevel = 1;
                        break;
                    }
                    zoomFactor = pow(1.2, zoomLevel) * 0.16150558288984572135;

                    view.setSize(globals::viewPortW*zoomFactor, globals::viewPortH*(float)zoomFactor);
                    view.setCenter(cursorWorld - viewCenterToCursorWorld * (float)zoomFactor);
                    window.setView(view);
                }
                break;
            default:
                break;
            }
        }

        window.clear();

        for (auto i=0llu; i<speed; ++i) {
            c.begin();
            world.tick();
            ++t;
            tickTime = c.end(Clock::MICROSECONDS);

            if (!(t%1000))
                printf("Tick time: %0.2f us\tspeed: %llu\n", tickTime, speed);
        }

        if (autoSpeed) {
            if (tickTime*speed < 27000)
                ++speed;
                //printf("Speed: %llu\n", ++speed);
            if (tickTime*speed > 37000 && speed > 1)
                --speed;
                //printf("Speed: %llu\n", --speed);
        }

        world.render(window);
        window.display();
    }

    return 0;
}
