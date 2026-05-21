#ifndef MAP_H
#define MAP_H

const int MAP_WIDTH = 80;
const int MAP_HEIGHT = 80;
const int VISIBILITY_RADIUS = 6;

struct Position
{
    int x;
    int y;
};

class Map
{
public:
    Map();
    void build();
    bool isWalkable(int x, int y) const;
    char tileAt(int x, int y) const;
    void copyTo(char destination[MAP_HEIGHT][MAP_WIDTH]) const;
    void revealArea(int centerX, int centerY, bool known[MAP_HEIGHT][MAP_WIDTH]) const;
    void getStart(Position& start) const;
    void getExit(Position& exit) const;
    void openDoor(int x, int y);

private:
    char tiles[MAP_HEIGHT][MAP_WIDTH];
    void fill(char tile);
    void carveRoom(int x1, int y1, int x2, int y2);
    void carveHorizontal(int x1, int x2, int y);
    void carveVertical(int y1, int y2, int x);
    void drawBorder();
};

#endif // MAP_H
