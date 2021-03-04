#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum class STATE{
  PLAYING,
  WIN,
  LOSE, 
  ROOM_CHANGE
};

struct Player
{
  explicit Player(Point pos);

  bool Moved() const;
  STATE ProcessInput(MovementDir dir, Image &screen);
  void Draw(Image &screen);
  void change_key() { key = !key; }

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 100, .a = 255};
  bool key = false;
  int move_speed = 1;
  bool changed_room = false;
  Pixel * data = nullptr;
  Pixel ** tiles = nullptr;
  int direction = 0;
  int move = 0;
};

#endif //MAIN_PLAYER_H
