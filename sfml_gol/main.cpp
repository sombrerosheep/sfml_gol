#include <iostream>
#include <SFML/Graphics.hpp>

// 60 fps
float maxFrameTime = 1 / 60;

void Input() {

}

void Update(float frameTimeInMilli) {

}

void Draw(sf::RenderWindow& window) {
  sf::CircleShape circle;
  circle.setPosition(150, 105);
  circle.setRadius(100);
  circle.setFillColor(sf::Color::Red);

  window.clear();
  window.draw(circle);
  window.display();
}
  
void main(char** argv, int arg) {
  sf::RenderWindow window(sf::VideoMode(800,600), "Hello");

  while (window.isOpen()) {

    sf::Clock gameClock;
    sf::Event event;
    while (window.pollEvent(event)) {
      
      sf::Time delta = gameClock.restart();
      // handle close events
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // keyboard escape = quit
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
          window.close();
        }
      }

      Input();
      Update(delta.asMilliseconds());
      Draw(window);

    }
  }
}
