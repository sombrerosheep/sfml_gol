#include <iostream>
#include <sstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

// game state
float generation_time = 0;
int generations_per_sec = 5;
float max_generation_time = 1.f / (float)generations_per_sec;
bool running = false;
bool started = false;
bool show_stats = true;
int generation = 0;
int population = 0;

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
  return (y * rowWidth) + x;
}

sf::Vector2f getPositionOfIndex(int index) {
  return sf::Vector2f(
    index % tile_cols,
    index / tile_cols
  );
}

int safeX(int x) {
  if (x < 0) {
    return tile_cols - 1;
  }

  if (x >= tile_cols) {
    return 0;
  }

  return x;
}

int safeY(int y) {
  if (y < 0) {
    return tile_rows - 1;
  }

  if (y >= tile_rows) {
    return 0;
  }

  return y;
}

struct Neighbors {
  Neighbors(int a, int d) {
    alive = a;
    dead = d;
  }
  int alive;
  int dead;
};

Neighbors getNeighbors(sf::Vector2f coords) {
  int x = coords.x;
  int y = coords.y;
  
  int alive = 0;

  if (game_board[getIndexOfPosition(safeX(x - 1), safeY(y - 1))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x), safeY(y - 1))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x + 1), safeY(y - 1))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x - 1), safeY(y))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x + 1), safeY(y))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x - 1), safeY(y + 1))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x), safeY(y + 1))].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[getIndexOfPosition(safeX(x + 1), safeY(y + 1))].previous == TILE_STATE::ALIVE) {
    alive++;
  }

  return Neighbors(alive, 8 - alive);
}

void Input(sf::Vector2f mouse_position) {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
    if (mouse_position.x < 0 || mouse_position.x > width - 1 || mouse_position.y < 0 || mouse_position.y > height - 1) {
      return;
    }

    int gridX = std::floor(mouse_position.x) / tile_size;
    int gridY = std::floor(mouse_position.y) / tile_size;

    game_board[getIndexOfPosition(gridX, gridY)].current = TILE_STATE::ALIVE;
  }
}

void Update(float frameTimeInMilli) {
  population = 0;
  generation++;

  if (started) {
    for (int i = 0; i < game_board.size(); i++) {
      Neighbors n = getNeighbors(getPositionOfIndex(i));
      if (game_board[i].previous == TILE_STATE::ALIVE) {
        // Any live cell with fewer than two live neighbors dies, as if by under population.
        if (n.alive < 2) {
          game_board[i].current = TILE_STATE::DEAD;
        }
        // Any live cell with two or three live neighbors lives on to the next generation.
        else if (n.alive == 2 || n.alive == 3) {
          game_board[i].current = TILE_STATE::ALIVE;
        }
        // Any live cell with more than three live neighbors dies, as if by overpopulation.
        else if (n.alive > 3) {
          game_board[i].current = TILE_STATE::DEAD;
        }
      }
      // Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
      else if (n.alive == 3) {
        game_board[i].current = TILE_STATE::ALIVE;
      }
    }
  }

  // Migrate state for next turn
  for (int i = 0; i < game_board.size(); i++) {
    game_board[i].previous = game_board[i].current;
    if (game_board[i].current == TILE_STATE::ALIVE) {
      population++;
    }
  }
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

  if (show_stats) {
    sf::Font statFont;
    sf::Text statText;

    statFont.loadFromFile("rm_typerighter_medium.ttf");
    statText.setFont(statFont);

    const char* runMsg = running ? "Running (Press P to pause)" : "Paused (Press P to resume)";

    std::stringstream sstream;
    sstream
      << runMsg << "\n"
      << "Press I to increase generation speed\n"
      << "Press O to decrease generation speed\n"
      << "\n"
      << generations_per_sec << " G/s\n"
      << "Population: " << population << "\n"
      << "Generation: " << generation;

    statText.setString(sstream.str());
    statText.setOrigin(0, 0);
    statText.setPosition(15, 5);

    window.draw(statText);
  }

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
  game_board[getIndexOfPosition(21, 14)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(21, 16)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(20, 15)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(22, 16)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(23, 16)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(24, 15)].current = TILE_STATE::ALIVE;

  game_board[getIndexOfPosition(54, 53)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(54, 54)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(54, 55)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(54, 56)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(58, 50)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(57, 51)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(58, 50)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(57, 52)].current = TILE_STATE::ALIVE;

  for (int i = 0; i < game_board.size(); i++) {
    if (game_board[i].current == TILE_STATE::ALIVE) {
      population++;
    }
  }
}
  
void main(char** argv, int arg) {
  sf::RenderWindow window(sf::VideoMode(width, height), "Hello");
  sf::Clock gameClock;
  sf::Event event;

  init();

  while (window.isOpen()) {

    sf::Time delta = gameClock.restart();
    max_generation_time = 1.f / (float)generations_per_sec;
    generation_time += delta.asSeconds();

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
        else if (event.key.code == sf::Keyboard::P) {
          started = true;
          running = !running;
        }
        else if (event.key.code == sf::Keyboard::S) {
          show_stats = !show_stats;
        }
        else if (event.key.code == sf::Keyboard::I) {
          generations_per_sec++;
        }
        else if (event.key.code == sf::Keyboard::O) {
          generations_per_sec--;
          if (generations_per_sec < 1) {
            generations_per_sec = 1;
          }
        }
      }
    }

    if (!started) {
      sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
      Input(mousePosition);
    }

    // maintain consistent generation speed
    if (generation_time > max_generation_time) {
      if (running) {
        Update(delta.asMilliseconds());
      }

      generation_time = 0;
    }

    Draw(window);

  }
}
