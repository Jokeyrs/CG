#include "Room.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Room::Room(const std::string &a_path)
{
    char tile;
    std::ifstream infile(a_path);
    // data = new Pixel[RoomSize * tileSize * RoomSize * tileSize]{};
    // for(int i = 0; i < RoomSize; ++i) {
    //     for (int j = 0; j < RoomSize; ++j) {
    //         infile >> tile;
    //         if (tile == ".") {
    //             data + tileSize * j + RoomSize * tileSize * i = (Pixel*)stbi_load('../Tiles/Floor.jpg', nullptr, nullptr, nullptr, sizeof(Pixel));
    //         }
    //     }
    // }
}


Room::~Room()
{
    stbi_image_free(data);
}