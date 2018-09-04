#include <iostream>
#include <sstream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#define DEBUG_DRAW


// 60 fps
const float max_frame_time = 1.f / 2.f;
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

  int ul = getIndexOfPosition(safeX(x - 1), safeY(y - 1));
  int uc = getIndexOfPosition(safeX(x), safeY(y - 1));
  int ur = getIndexOfPosition(safeX(x + 1), safeY(y - 1));
  int l = getIndexOfPosition(safeX(x - 1), safeY(y));
  int r = getIndexOfPosition(safeX(x + 1), safeY(y));
  int ll = getIndexOfPosition(safeX(x - 1), safeY(y + 1));
  int lc = getIndexOfPosition(safeX(x), safeY(y + 1));
  int lr = getIndexOfPosition(safeX(x + 1), safeY(y + 1));

  if (game_board[ul].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[uc].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[ur].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[l].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[r].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[ll].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[lc].previous == TILE_STATE::ALIVE) {
    alive++;
  }
  if (game_board[lr].previous == TILE_STATE::ALIVE) {
    alive++;
  }

  return Neighbors(alive, 8 - alive);
}

void Input() {

}

void Update(float frameTimeInMilli) {
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

  // Migrate state for next turn
  for (int i = 0; i < game_board.size(); i++) {
    game_board[i].previous = game_board[i].current;
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
  game_board[getIndexOfPosition(21, 14)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(21, 16)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(20, 15)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(22, 16)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(23, 16)].current = TILE_STATE::ALIVE;
  game_board[getIndexOfPosition(24, 15)].current = TILE_STATE::ALIVE;
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
