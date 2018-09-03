#include <iostream>
#include <sstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#define DEBUG_DRAW


// 60 fps
const float max_frame_time = 1.f / 60.f;
float frame_time = 0;

// Seeded constants
const int height = 600;
const int width = 800;
const int tile_size = 10;
const sf::Color GRID_COLOR{ 48, 48, 48 };

// calculated constants
const int tile_rows = height / tile_size;
const int tile_cols = width / tile_size;

// Gridlines
const int grid_vert_verticies = tile_cols * 2;
const int grid_horz_verticies = tile_rows * 2;
sf::Vertex grid_vert_lines[grid_vert_verticies];
sf::Vertex grid_horz_lines[grid_horz_verticies];

enum TILE_STATE {
  DEAD = 0,
  ALIVE
};

struct Tile {
  Tile() {
    current = TILE_STATE::DEAD;
    previous = TILE_STATE::DEAD;
  }

  TILE_STATE current;
  TILE_STATE previous;
};

// gameboard
std::vector<Tile> game_board(tile_rows * tile_cols);

int getIndexOfPosition(int x, int y, int rowWidth = tile_cols) {
  return y * rowWidth + x;
}

sf::Vector2f getPositionOfIndex(int index) {
  return sf::Vector2f(
    index % tile_cols,
    index / tile_cols
  );
}

void Input() {

}

void Update(float frameTimeInMilli) {
  
}

void Draw(sf::RenderWindow& window) {

  window.clear(sf::Color::Black);

  // Draw the grid
  window.draw(grid_horz_lines, grid_horz_verticies, sf::Lines);
  window.draw(grid_vert_lines, grid_vert_verticies, sf::Lines);

  for (int i = 0; i < game_board.size(); i++) {
    if (game_board[i].current == TILE_STATE::ALIVE) {
      sf::RectangleShape rect(sf::Vector2f(tile_size, tile_size));
      sf::Vector2f pos = getPositionOfIndex(i);
      rect.setPosition(pos.x * tile_size, pos.y * tile_size);
      window.draw(rect);
    }
  }

#ifdef DEBUG_DRAW
  sf::Font debugFont;
  sf::Text fpsText;
 
  debugFont.loadFromFile("rm_typerighter_medium.ttf");
  fpsText.setFont(debugFont);

  std::stringstream sstream;
  sstream << (int)(1 / frame_time) << " FPS";

  fpsText.setString(sstream.str());
  fpsText.setOrigin(0, 0);
  fpsText.setPosition(15, 5);

  window.draw(fpsText);
#endif

  window.display();
}

void init() {
  // create grid
  for (int i = 0; i < grid_horz_verticies; i += 2) {
    grid_horz_lines[i].position = sf::Vector2f(0, i * (tile_size / 2));
    grid_horz_lines[i].color = GRID_COLOR;
    grid_horz_lines[i + 1].position = sf::Vector2f(width, i * (tile_size / 2));
    grid_horz_lines[i + 1].color = GRID_COLOR;
  }

  for (int i = 0; i < grid_vert_verticies; i += 2) {
    grid_vert_lines[i].position = sf::Vector2f(i * (tile_size / 2), 0);
    grid_vert_lines[i].color = GRID_COLOR;
    grid_vert_lines[i + 1].position = sf::Vector2f(i * (tile_size / 2), height);
    grid_vert_lines[i + 1].color = GRID_COLOR;
  }

  // clear the gameboard
  for (int i = 0; i < game_board.size(); i++) {
    game_board[i] = {};
  }

  // Seed the board
  game_board[getIndexOfPosition(20, 10)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(21, 11)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(20, 12)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(20, 13)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(21, 9)].current = TILE_STATE::ALIVE;
}
  
void main(char** argv, int arg) {
  sf::RenderWindow window(sf::VideoMode(width, height), "Hello");
  sf::Clock gameClock;
  sf::Event event;

  init();

  while (window.isOpen()) {

    sf::Time delta = gameClock.restart();
    frame_time += delta.asSeconds();

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
    if (frame_time > max_frame_time) {
      Input();
      Update(delta.asMilliseconds());
      Draw(window);

      frame_time = 0;
    }

  }
}
