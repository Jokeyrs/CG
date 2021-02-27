#ifndef MAIN_ROOM_H
#define MAIN_ROOM_H

#include <string>

#include <fstream>
#include <iostream>

constexpr const int tileSize = 16;

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

constexpr int RoomSize = 64;

struct Room {
    explicit Room(const std::string &a_path);
    Pixel * get_room() { return data; };
    ~Room();
private:
    Pixel *data = nullptr;
};


#endif //MAIN_ROOM_H