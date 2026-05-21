#include <cstring>
#include <map.h>

Map::Map()
{
    build();
}

void Map::build()
{
    fill('#');
    drawBorder();

    // Top row: Room1, Room2, Room3
    carveRoom(2, 2, 24, 14);   // Room 1 (top-left)
    carveRoom(27, 2, 49, 14);  // Room 2 (top-center)
    carveRoom(52, 2, 74, 14);  // Room 3 (top-right)

    // Bottom row: Room6 (boss), Room5, Room4 (under the top row, right-to-left)
    carveRoom(2, 17, 24, 29);  // Room 6 (bottom-left, boss)
    carveRoom(27, 17, 49, 29); // Room 5 (bottom-center)
    carveRoom(52, 17, 74, 29); // Room 4 (bottom-right)

    // =========================
    // CONNECTIONS / DOORS (new layout)
    // =========================

    // Top row horizontal connections (Room1 <-> Room2, Room2 <-> Room3)
    // Between Room1 (x 2..24) and Room2 (x 27..49)
    tiles[8][25] = ' ';
    tiles[8][26] = ' ';

    // Between Room2 (x 27..49) and Room3 (x 52..74)
    tiles[8][50] = ' ';
    tiles[8][51] = ' ';

    // Vertical connection: Room3 (top-right) down to Room4 (bottom-right)
    // This starts closed and will be opened when the player reaches Room 3
    tiles[15][63] = '(';
    tiles[16][63] = ')';

    // Bottom row horizontal connections (Room4 <-> Room5 <-> Room6)
    // Between Room4 (x 52..74) and Room5 (x 27..49)
    tiles[23][50] = ' ';
    tiles[23][51] = ' ';

    // Between Room5 (x 27..49) and Room6 (x 2..24)
    tiles[23][25] = ' ';
    tiles[23][26] = ' ';

    // Exit
    tiles[42][48] = 'X';
}
bool Map::isWalkable(int x, int y) const
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return false;
    }

    char tile = tiles[y][x];

    return tile == ' ' || tile == 'X';
}

char Map::tileAt(int x, int y) const
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return '#';
    }

    return tiles[y][x];
}

void Map::copyTo(char destination[MAP_HEIGHT][MAP_WIDTH]) const
{
    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            destination[y][x] = tiles[y][x];
        }
    }
}

void Map::revealArea(int centerX, int centerY, bool known[MAP_HEIGHT][MAP_WIDTH]) const
{
    for (int y = centerY - VISIBILITY_RADIUS; y <= centerY + VISIBILITY_RADIUS; y = y + 1)
    {
        for (int x = centerX - VISIBILITY_RADIUS; x <= centerX + VISIBILITY_RADIUS; x = x + 1)
        {
            if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
            {
                continue;
            }

            int dx = x - centerX;
            int dy = y - centerY;

            int distanceSquared = dx * dx + dy * dy;

            if (distanceSquared <= VISIBILITY_RADIUS * VISIBILITY_RADIUS)
            {
                known[y][x] = true;
            }
        }
    }
}

void Map::getStart(Position& start) const
{
    start.x = 3;
    start.y = 3;
}

void Map::getExit(Position& exit) const
{
    exit.x = MAP_WIDTH - 2;
    exit.y = MAP_HEIGHT - 2;
}

void Map::openDoor(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return;
    }

    if (tiles[y][x] == '(' || tiles[y][x] == ')')
    {
        tiles[y][x] = ' ';

        if (x > 0 && (tiles[y][x - 1] == '(' || tiles[y][x - 1] == ')'))
        {
            tiles[y][x - 1] = ' ';
        }

        if (x + 1 < MAP_WIDTH && (tiles[y][x + 1] == '(' || tiles[y][x + 1] == ')'))
        {
            tiles[y][x + 1] = ' ';
        }

        if (y > 0 && (tiles[y - 1][x] == '(' || tiles[y - 1][x] == ')'))
        {
            tiles[y - 1][x] = ' ';
        }

        if (y + 1 < MAP_HEIGHT && (tiles[y + 1][x] == '(' || tiles[y + 1][x] == ')'))
        {
            tiles[y + 1][x] = ' ';
        }

        return;
    }

    if (tiles[y][x] == '#')
    {
        tiles[y][x] = ' ';
    }
}

void Map::fill(char tile)
{
    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            tiles[y][x] = tile;
        }
    }
}

void Map::drawBorder()
{
    for (int x = 0; x < MAP_WIDTH; x = x + 1)
    {
        tiles[0][x] = '#';
        tiles[MAP_HEIGHT - 1][x] = '#';
    }

    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        tiles[y][0] = '#';
        tiles[y][MAP_WIDTH - 1] = '#';
    }
}

void Map::carveRoom(int x1, int y1, int x2, int y2)
{
    for (int y = y1; y <= y2; y = y + 1)
    {
        for (int x = x1; x <= x2; x = x + 1)
        {
            tiles[y][x] = ' ';
        }
    }
}

void Map::carveHorizontal(int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x = x + 1)
    {
        tiles[y][x] = ' ';
    }
}

void Map::carveVertical(int y1, int y2, int x)
{
    for (int y = y1; y <= y2; y = y + 1)
    {
        tiles[y][x] = ' ';
    }
}
