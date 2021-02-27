#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <fstream>

Image::Image() {
  Room room("../Rooms/room1.txt");
  data = room.get_room();

  Room background_room("../Rooms/room1.txt");
  background_data = background_room.get_room();
  width = roomSize * tileSize;
  height = roomSize * tileSize;
  channels = sizeof(Pixel);
  size = width * height * channels;
}

Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, sizeof(Pixel))) != nullptr)
  {
    background_data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, sizeof(Pixel));
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};
  background_data = new Pixel[a_width * a_height ]{};

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
  data = new Pixel[roomSize * tileSize * roomSize * tileSize]{};
  for (int i = 0; i < roomSize; ++i) {
    for (int j = 0; j < roomSize; ++j) {
      infile >> tile;

      int tmp_width;
      int tmp_height;
      int tmp_channels;
      Pixel * tmp = nullptr;

      switch(tile){
        case '.':
          tmp = (Pixel*)stbi_load("../Tiles/Floor.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        case '#':
          tmp = (Pixel*)stbi_load("../Tiles/Wall.jpg", &tmp_width, &tmp_channels, &tmp_height, sizeof(Pixel));
          break;
        default:
          tmp = new Pixel[tileSize * tileSize]{};
      }
        
      for (int k = 0; k < tileSize; ++k) {
        for (int t = 0; t < tileSize; ++t) {
          data[(i * tileSize + k) * roomSize * tileSize + j * tileSize + t] = tmp[k * tileSize + t];
        }
      }
    }
  }
}


Room::~Room()
{
}