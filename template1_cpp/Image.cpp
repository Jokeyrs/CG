#include "Image.h"
#include "Player.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <fstream>

std::string room_files[6] = {"../Rooms/room1.txt", "../Rooms/room2.txt", "../Rooms/room3.txt", "../Rooms/room4.txt", "../Rooms/room5.txt", "../Rooms/room6.txt"};
const char * tile_files[12] = {"../Tiles/Hero_Down_Stay.jpg", "../Tiles/Hero_Down_Left.jpg", "../Tiles/Hero_Down_Right.jpg",
                              "../Tiles/Hero_Left_Stay.jpg", "../Tiles/Hero_Left_Left.jpg", "../Tiles/Hero_Left_Right.jpg",
                              "../Tiles/Hero_Up_Stay.jpg", "../Tiles/Hero_Up_Left.jpg", "../Tiles/Hero_Up_Right.jpg",
                              "../Tiles/Hero_Right_Stay.jpg", "../Tiles/Hero_Right_Left.jpg", "../Tiles/Hero_Right_Right.jpg",};

Player::Player(Point pos) : coords(pos), old_coords(coords) {
  tiles = new Pixel*[12];
  int tmp_width;
  int tmp_height;
  int tmp_channels;

  for (int i = 0; i < 12; ++i) {
    tiles[i] =  (Pixel*)stbi_load(tile_files[i], &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
    // tiles[i] = new Pixel[16*16];
  }

  data = tiles[0];
}

Image::Image(const std::string &a_path) {
  rooms = new struct Room[mapWidth * mapHeight];

  char room_c;
  std::ifstream infile(a_path);
  for (int i = 0; i < mapHeight; ++i) {
    for (int j = 0; j < mapWidth; ++j) {
      infile >> room_c;
      rooms[i * mapWidth + j].init(room_files[room_c - 'A']);
    }
  }

  cur_room = &rooms[y_room * mapWidth + x_room];

  data = new Pixel[block_size * block_size * roomSize * roomSize];
  Pixel * room_data = cur_room->get_room();
  for (int i = 0; i < block_size * block_size * roomSize * roomSize; ++i) {
    data[i] = room_data[i];
  }

  width = roomSize * block_size;
  height = roomSize * block_size;
  channels = sizeof(Pixel);
  size = width * height * channels;
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}


int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

void Image::change_room(int dir) {

  switch(dir) {
    case 0:
      x_room -= 1;
      break;
    case 1:
      y_room -= 1;
      break;
    case 2:
      x_room += 1;
      break;
    case 3:
      y_room += 1;
      break;
    default:
      break;
  }

  std::cout << x_room << " " << y_room << std::endl;

  prev_room = cur_room;
  cur_room = &rooms[y_room * mapWidth + x_room];
  Pixel * room_data = cur_room->get_room();

  // for (int i = 0; i < block_size * block_size * roomSize * roomSize; ++i) {
  //   data[i] = room_data[i];
  // }
} 

void Image::blend(float alpha) {
  Pixel * cur_room_data = cur_room->get_room();
  Pixel * prev_room_data = prev_room->get_room();

  for (int i = 0; i < roomSize * roomSize * block_size * block_size; ++i) {
    data[i].r = cur_room_data[i].r * alpha + prev_room_data[i].r * (1 - alpha);
    data[i].g = cur_room_data[i].g * alpha + prev_room_data[i].g * (1 - alpha);
    data[i].b = cur_room_data[i].b * alpha + prev_room_data[i].b * (1 - alpha);
    data[i].a = cur_room_data[i].a * alpha + prev_room_data[i].a * (1 - alpha);
  }
}

void Image::winData() {
  int tmp_width;
  int tmp_height;
  int tmp_channels;
  data = (Pixel*)stbi_load("../Tiles/Win.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
  return;
}

void Image::loseData() {
  int tmp_width;
  int tmp_height;
  int tmp_channels;
  data = (Pixel*)stbi_load("../Tiles/Lose.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
  return;
}

Pixel * Image::floorData() {
  int tmp_width;
  int tmp_height;
  int tmp_channels;
  Pixel * tmp = (Pixel*)stbi_load("../Tiles/Floor.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
  return tmp;
}


Image::~Image() {
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}

Room::Room(const std::string &a_path) {
  Room::init(a_path);
}

void Room::init(const std::string &a_path) {
  char tile;
  std::ifstream infile(a_path);
  data = new Pixel[block_size * block_size * roomSize * roomSize]{};
  for (int i = 0; i < roomSize; ++i) {
    for (int j = 0; j < roomSize; ++j) {
      infile >> tile;

      int tmp_width;
      int tmp_height;
      int tmp_channels;
      Pixel * tmp = nullptr;
      map[i * roomSize + j] = tile;

      switch(tile){
        case '.':
          tmp = (Pixel*)stbi_load("../Tiles/Floor.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case 'x':
          tmp = (Pixel*)stbi_load("../Tiles/Door.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case 'Q':
          tmp = (Pixel*)stbi_load("../Tiles/Quit.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case '#':
          tmp = (Pixel*)stbi_load("../Tiles/Wall.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case '_':
          tmp = (Pixel*)stbi_load("../Tiles/Empty.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case 'K':
          tmp = (Pixel*)stbi_load("../Tiles/MainKey.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        default:
          tmp = new Pixel[block_size * block_size]{};
      }
        
      for (int k = 0; k < block_size; ++k) {
        for (int t = 0; t < block_size; ++t) {
          data[(i * block_size + k) * roomSize * block_size + j * block_size + t] = tmp[k * block_size + t];
        }
      }
    }
  }
}


Room::~Room() {
  delete [] data;
}