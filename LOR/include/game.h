#ifndef GAME_H
#define GAME_H

#include "map.h"
#include "entity.h"
#include <vector>
#include <windows.h>

const int MAX_ENEMIES = 6;
const int MAX_ITEMS = 12;

class Game
{
public:
    Game();
    void initialize();
    void run();

private:
    Map map;
    Player player;
    Entity enemies[MAX_ENEMIES];
    Item items[MAX_ITEMS];
    bool known[MAP_HEIGHT][MAP_WIDTH];
    bool running;
    bool inGame;
    bool victory;
    bool defeat;
    bool loadRequested;
    bool needRedraw;
    HANDLE consoleHandle;
    int screenWidth;
    int screenHeight;
    std::vector<char> screenChars;
    std::vector<WORD> screenAttrs;
    int currentRoom;
    int score;
    int enemyTick;
    char status[128];
    int frameCount;
    Position lastPlayerPos;

    void drawControls();
    void drawRoomTitle();
    void killNearbyEnemies();

    void initializeFog();
    void initializePlayer();
    void initializeEntities();
    void initializeItems();
    void updateFog();
    void draw();
    void drawMap();
    void drawHUD();
    void drawMinimap();
    void drawDialogue();
    void showMenu();
    void showInstructions();
    void processInput();
    void movePlayer(int dx, int dy);
    void updateCurrentRoom();
    void updateEnemies();
    void updateEnemy(Entity* enemy);
    void checkCollisions();
    void checkItemPickup();
    void saveGame();
    bool loadGame();
    void setStatus(const char* message);
    void beepShort();
    void beepLong();
    bool canSee(int x, int y) const;
};

#endif // GAME_H
