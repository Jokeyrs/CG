#include "Player.h"
// #include "Image.cpp"

#include <iostream>

// char * tile_files[4] = {"../Tiles/Hero_Down_Stay.jpg", "../Tiles/Hero_Left_Stay.jpg", "../Tiles/Hero_Up_Stay.jpg", "../Tiles/Hero_Right_Stay.jpg"};
// std::string left = "../Tiles/Hero_Down_Stay.jpg";

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

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
      int y = coords.y + i * tileSize - i;
      int x = coords.x + j * tileSize - j;
      ans |= (map[y / block_size * roomSize + x / block_size] == '#') | (map[y / block_size * roomSize + x / block_size] == 'x');
      ans |= (x < 0) || (x >= roomSize * block_size) || (y < 0) || (y >= roomSize * block_size);
    } 
  }
  return ans;
}

bool check_quit(Point &coords, char *map) {
  int y = coords.y;
  int x = coords.x;
  return (map[y / block_size * roomSize + x / block_size] == 'Q');
}

bool check_key(Point &coords, Image& screen) {
  char * map = screen.Room()->Map();
  Pixel * room_data = screen.Room()->get_room();
  Pixel * data = screen.Data();
  int tmp_width;
  int tmp_height;
  int tmp_channels;

  for (int x = 0; x < 2; ++x) { 
    for (int y = 0; y < 2; ++y) {
      int i = (coords.y + tileSize / 4 + x * tileSize / 2) / block_size;
      int j = (coords.x + tileSize / 4 + y * tileSize / 2) / block_size;
      if (map[i * roomSize + j] == 'K') {
        map[i * roomSize + j] = '.';
        Pixel * tmp = screen.floorData();
        for (int k = 0; k < block_size; ++k) {
          for (int t = 0; t < block_size; ++t) {
            int y_coord = (i * block_size + k) * block_size;
            int x_coord = j * block_size + t;

            if (!((x_coord < coords.x + tileSize) && (x_coord >= coords.x) && (y_coord < coords.y + tileSize) && y_coord >= coords.y)) {
              data[y_coord * block_size + x_coord] = tmp[k * block_size + t];
            }
            room_data[y_coord * block_size + x_coord] = tmp[k * block_size + t];
          }
        }
        return true;
      }
    } 
  }
  return false;
}

void break_wall(Point &coords, Image& screen) {
  char * map = screen.Room()->Map();
  Pixel * room_data = screen.Room()->get_room();
  Pixel * data = screen.Data();
  int tmp_width;
  int tmp_height;
  int tmp_channels;

  for (int x = 0; x < 2; ++x) { 
    for (int y = 0; y < 2; ++y) {
      int i = (coords.y - 1 + y * (tileSize + 2)) / block_size;
      int j = (coords.x - 1 + x * (tileSize + 2)) / block_size;
      if (map[i * roomSize + j] == 'x') {
        map[i * roomSize + j] = '.';
        Pixel * tmp = screen.floorData();
        for (int k = 0; k < block_size; ++k) {
          for (int t = 0; t < block_size; ++t) {
            int y_coord = (i * block_size + k) * block_size;
            int x_coord = j * block_size + t;
            data[y_coord * block_size + x_coord] = tmp[k * block_size + t];
            room_data[y_coord * block_size + x_coord] = tmp[k * block_size + t];
          }
        }
        return;
      }
    } 
  }
  return;
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

STATE Player::ProcessInput(MovementDir dir, Image &screen) {
  char * room_map = screen.Room()->Map();
  int move_dist = move_speed * 1;

  // std::cout << coords.x << " " << coords.y << std::endl;

  /*preprocess room change*/
  bool is_change = false;

  if (coords.x + tileSize == block_size * roomSize - 1) { 
    coords.x = tileSize / 2;
    screen.change_room(2);
    is_change = true;
  } else if (coords.x == 0) { 
    coords.x = block_size * roomSize - tileSize - tileSize / 2;
    screen.change_room(0);
    is_change = true;
  } else if (coords.y + tileSize == block_size * roomSize - 1) {
    coords.y = tileSize / 2;
    screen.change_room(1);
    is_change = true;
  } else if (coords.y == 0) { 
    coords.y = block_size * roomSize - tileSize - tileSize / 2;
    screen.change_room(3);
    is_change = true;
  }

  if (is_change) {
    old_coords.x = coords.x;
    old_coords.y = coords.y;
    key = false;
    return STATE::ROOM_CHANGE;
  }

  int new_move;

  while (move_dist > 0) {
    Point new_coords{coords.x, coords.y};

    switch(dir)
    {
      case MovementDir::UP:
        new_coords.y += move_dist;
        direction = 2;
        new_move = move % 2 + 1;
        break;
      case MovementDir::DOWN:
        new_coords.y -= move_dist;
        direction = 0;
        new_move = move % 2 + 1;
        break;
      case MovementDir::LEFT:
        new_coords.x -= move_dist;
        direction = 1;
        new_move = move % 2 + 1;
        break;
      case MovementDir::RIGHT:
        new_coords.x += move_dist;
        direction = 3;
        new_move = move % 2 + 1;
        break;
      default:
        new_move = 0;
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

    move_dist -= 1;
  }

  data = tiles[direction * 3 + new_move];
  move = new_move;

  if (check_quit(coords, room_map)) {
    return STATE::WIN;
  }

  if (check_empty(coords, room_map)) {
    return STATE::LOSE;
  }

  if (check_key(coords, screen)) {
    key = true;
    std::cout << "Picked a key" << std::endl;
  }

  if (key) {
    break_wall(coords, screen);
  } 
  
  return STATE::PLAYING;
}

static Pixel blend(Pixel oldPixel, Pixel newPixel) {
  if (!newPixel.r && !newPixel.g && !newPixel.b && !newPixel.a) {
    // std::cout << "Im here" << std::endl;
    return oldPixel;
  }
  return newPixel;
}

void Player::Draw(Image &screen)
{
  if(Moved())
  {
    for(int y = old_coords.y; y < old_coords.y + tileSize; ++y)
    {
      for(int x = old_coords.x; x < old_coords.x + tileSize; ++x)
      {
        screen.PutBackGround(x, y);
      }
    }
    old_coords = coords;
  }

  for(int y = coords.y; y < coords.y + tileSize; ++y)
  {
    for(int x = coords.x; x < coords.x + tileSize; ++x)
    {
      Pixel tmp = data[(coords.y + tileSize - y) * tileSize + (x - coords.x)];
      if (tmp.r > 50 || tmp.g  > 50 || tmp.b > 50) {
        screen.PutPixel(x, y, tmp);
      }
    }
  }
}