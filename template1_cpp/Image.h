#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>

constexpr int block_size = 32;
constexpr int tileSize = 16;
constexpr int roomSize = 32;

constexpr int mapWidth = 10;
constexpr int mapHeight = 2;

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

constexpr Pixel backgroundColor{0, 0, 0, 0};

struct Room {
    explicit Room(const std::string &a_path);
    Room() {};
    Pixel * get_room() { return data; };
    char * Map() { return map; }
    ~Room();
    void init(const std::string &a_path);
private:
    Pixel *data = nullptr;
    char * map = new char[roomSize * roomSize];
};

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return  data; }
  struct Room * Room()        { return cur_room; }
  void change_room(int dir);
  void winData();
  void loseData();
  Pixel * floorData();


  Pixel GetPixel(int x, int y) { return data[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[width* y + x] = pix; }
  void  PutBackGround(int x, int y) { data[width* y + x] = cur_room->get_room()[width * y + x]; }

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
  struct Room * cur_room = nullptr;
  char * rooms_map = new char[2];
  struct Room * rooms = nullptr;
  int x_room = 0;
  int y_room = 0;
};


#endif //MAIN_IMAGE_H
