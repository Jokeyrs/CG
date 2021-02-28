#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <fstream>

Image::Image() {
  room = new struct Room("../Rooms/room1.txt");

  data = new Pixel[block_size * block_size * roomSize * roomSize];
  Pixel * room_data = room->get_room();
  for (int i = 0; i < block_size * block_size * roomSize * roomSize; ++i) {
    data[i] = room_data[i];
  }

  width = roomSize * block_size;
  height = roomSize * block_size;
  channels = sizeof(Pixel);
  size = width * height * channels;
}

Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, sizeof(Pixel))) != nullptr)
  {
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};

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

Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}


Room::Room(const std::string &a_path)
{
  char tile;
  std::ifstream infile(a_path);
  data = new Pixel[roomSize * block_size * roomSize * block_size]{};
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
        case 'Q':
          tmp = (Pixel*)stbi_load("../Tiles/Floor.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case '#':
          tmp = (Pixel*)stbi_load("../Tiles/Wall.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case '_':
          tmp = (Pixel*)stbi_load("../Tiles/Empty.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
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


Room::~Room()
{
}