#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#define DEBUG_DRAW


// 60 fps
const float maxFrameTime = 1.f / 60.f;
float frameTime = 0;
const int height = 600;
const int width = 800;
sf::CircleShape circle;
sf::Vector2f circleVelocity(1, 1);
float circleSpeed = 3.f;

void Input() {

}

void Update(float frameTimeInMilli) {
  circle.move(circleVelocity * circleSpeed);

  sf::Vector2f circlePos = circle.getPosition();
  sf::FloatRect bounds = circle.getGlobalBounds();

  if (circlePos.x > width - bounds.width) {
    circlePos.x = width - bounds.width;
    circleVelocity.x *= -1;
  }
  if (circlePos.x < 0) {
    circlePos.x = 0;
    circleVelocity.x *= -1;
  }

  if (circlePos.y > height - bounds.height) {
    circlePos.y = height - bounds.height;
    circleVelocity.y *= -1;
  }
  if (circlePos.y < 0) {
    circlePos.y = 0;
    circleVelocity.y *= -1;
  }

  circle.setPosition(circlePos);
}

void Draw(sf::RenderWindow& window) {

  window.clear();
  window.draw(circle);

#ifdef DEBUG_DRAW
  sf::Font debugFont;
  sf::Text fpsText;
 
  debugFont.loadFromFile("rm_typerighter_medium.ttf");
  fpsText.setFont(debugFont);

  std::stringstream sstream;
  sstream << (int)(1 / frameTime) << " FPS";

  fpsText.setString(sstream.str());
  fpsText.setOrigin(0, 0);
  fpsText.setPosition(15, 5);

  window.draw(fpsText);
#endif

  window.display();
}

void init() {
  circle.setPosition(150, 105);
  circle.setRadius(100);
  circle.setFillColor(sf::Color::Red);
}
  
void main(char** argv, int arg) {
  sf::RenderWindow window(sf::VideoMode(width, height), "Hello");
  sf::Clock gameClock;
  sf::Event event;

  init();

  while (window.isOpen()) {

    sf::Time delta = gameClock.restart();
    frameTime += delta.asSeconds();

    while (window.pollEvent(event)) {
      
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
    }

    // maintain consistent frametime
    if (frameTime > maxFrameTime) {
      Input();
      Update(delta.asMilliseconds());
      Draw(window);

      frameTime = 0;
    }

  }
}
