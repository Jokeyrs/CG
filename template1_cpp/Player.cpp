#include "Player.h"


bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

bool check_walls(Point &coords, char * map) {  
  bool ans = false;
  for (int i = 0; i < 2; ++i) { 
    for (int j = 0; j < 2; ++j) {
      int y = coords.y + i * tileSize;
      int x = coords.x + j * tileSize;
      ans |= (map[y / block_size * roomSize + x / block_size] == '#');
    } 
  }
  return ans;
}

bool check_quit(Point &coords, char *map) {
  int y = coords.y;
  int x = coords.x;
  return (map[y / block_size * roomSize + x / block_size] == 'Q');
}

bool check_empty(Point &coords, char *map) {
  bool ans = false;
  for (int i = 0; i < 2; ++i) { 
    for (int j = 0; j < 2; ++j) {
      int y = coords.y + tileSize / 4 + i * tileSize / 2;
      int x = coords.x + tileSize / 4 + j * tileSize / 2;
      ans |= (map[y / block_size * roomSize + x / block_size] == '_');
    } 
  }
  return ans;
}

STATE Player::ProcessInput(MovementDir dir, Image &screen)
{
  char * room_map = screen.Room()->Map();
  int move_dist = move_speed * 1;

  while (move_dist > 0) {
    Point new_coords{coords.x, coords.y};
    move_dist -= 1;

    switch(dir)
    {
      case MovementDir::UP:
        new_coords.y += move_dist;
        break;
      case MovementDir::DOWN:
        new_coords.y -= move_dist;
        break;
      case MovementDir::LEFT:
        new_coords.x -= move_dist;
        break;
      case MovementDir::RIGHT:
        new_coords.x += move_dist;
        break;
      default:
        break;
    }

    if (!check_walls(new_coords, room_map)) {
      if (new_coords.x != coords.x) {
        old_coords.x = coords.x;
        coords.x = new_coords.x;
      } else if (new_coords.y != coords.y) {
        old_coords.y = coords.y;
        coords.y = new_coords.y;
      }
      break;
    }
  }

  if (check_quit(coords, room_map)) {
    return STATE::WIN;
  }

  if (check_empty(coords, room_map)) {
    return STATE::LOSE;
  }
  
  return STATE::PLAYING;
}

void Player::Draw(Image &screen)
{
  if(Moved())
  {
    for(int y = old_coords.y; y <= old_coords.y + tileSize; ++y)
    {
      for(int x = old_coords.x; x <= old_coords.x + tileSize; ++x)
      {
        screen.PutBackGround(x, y);
      }
    }
    old_coords = coords;
  }

  for(int y = coords.y; y <= coords.y + tileSize; ++y)
  {
    for(int x = coords.x; x <= coords.x + tileSize; ++x)
    {
      screen.PutPixel(x, y, color);
    }
  }
}