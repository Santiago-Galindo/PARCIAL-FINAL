#include <conio.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <cstring>
#include <string>
#include <game.h>

Game::Game()
{
    running = false;
    inGame = false;
    victory = false;
    defeat = false;
    loadRequested = false;
    frameCount = 0;
    lastPlayerPos.x = 0;
    lastPlayerPos.y = 0;
    setStatus("Bienvenido a la Aventura Retro: encuentra la libreta perdida.");
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(consoleHandle, &cursorInfo))
    {
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(consoleHandle, &cursorInfo);
    }
    needRedraw = false;
    currentRoom = 1;
    score = 0;
    enemyTick = 0;
    // buffered screen: map + minimap + controls
    int minimapW = 18;
    screenWidth = MAP_WIDTH + 2 + minimapW;
    int controlsH = 21;
    screenHeight = MAP_HEIGHT + 4 + controlsH; // dialogue + title + map + controls
    screenChars.resize(screenWidth * screenHeight);
    screenAttrs.resize(screenWidth * screenHeight);

    COORD bufSize;
    bufSize.X = (SHORT)screenWidth;
    bufSize.Y = (SHORT)screenHeight;
    SetConsoleScreenBufferSize(consoleHandle, bufSize);
    SMALL_RECT winRect;
    winRect.Left = 0;
    winRect.Top = 0;
    winRect.Right = (SHORT)(screenWidth - 1);
    winRect.Bottom = (SHORT)(screenHeight - 1);
    SetConsoleWindowInfo(consoleHandle, TRUE, &winRect);
    SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void Game::initialize()
{
    map.build();
    initializePlayer();
    initializeFog();
    initializeEntities();
    initializeItems();
    victory = false;
    defeat = false;
    running = true;
    frameCount = 0;
    setStatus("Nueva partida iniciada. Busca la espada y descubre el dungeon.");
    needRedraw = true;
}

void Game::run()
{
    while (true)
    {
        showMenu();
        if (inGame == false)
        {
            break;
        }
        if (loadRequested == true)
        {
            bool loaded = loadGame();
            if (loaded == false)
            {
                initialize();
                setStatus("No se encontró partida. Empieza una nueva aventura.");
            }
            else
            {
                setStatus("Partida cargada. Continúa explorando la oscuridad.");
            }
        }
        else
        {
            initialize();
        }

        while (running)
        {
            if (needRedraw)
            {
                draw();
                needRedraw = false;
            }
            processInput();
            enemyTick = enemyTick + 1;
            // move enemies only every N ticks to slow them down
            if (enemyTick % 3 == 0)
            {
                updateEnemies();
            }
            checkCollisions();
            updateFog();
            Sleep(120);
            frameCount = frameCount + 1;
        }

        draw();
        if (victory)
        {
            setStatus("¡Has vencido! La libreta está a salvo y el jefe final ha caído.");
            beepLong();
        }
        else if (defeat)
        {
            setStatus("Has sido derrotado. La historia termina en la penumbra.");
            beepLong();
        }
        draw();
        Sleep(1200);
    }
}

void Game::initializeFog()
{
    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            known[y][x] = false;
        }
    }
}

void Game::initializePlayer()
{
    map.getStart(player.position);
    player.hp = 10;
    player.hasSword = false;
    player.hasTreasure = false;
    player.hasItem = false;
    player.carriedItem = ' ';
    lastPlayerPos = player.position;
}

void Game::initializeEntities()
{
    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        enemies[index].active = false;
    }

    enemies[0].position.x = 10;
    enemies[0].position.y = 10;
    enemies[0].symbol = '!';
    enemies[0].hp = 1;
    enemies[0].type = EnemyType::Chaser;
    enemies[0].active = true;
    enemies[0].isBoss = false;
    enemies[0].isDouble = false;

    enemies[1].position.x = 40;
    enemies[1].position.y = 10;
    enemies[1].symbol = '!';
    enemies[1].hp = 1;
    enemies[1].type = EnemyType::Stalker;
    enemies[1].active = true;
    enemies[1].isBoss = false;
    enemies[1].isDouble = true;

    enemies[2].position.x = 10;
    enemies[2].position.y = 22;
    enemies[2].symbol = 'B';
    enemies[2].hp = 4;
    enemies[2].type = EnemyType::Boss;
    enemies[2].active = true;
    enemies[2].isBoss = true;
    enemies[2].isDouble = false;
}

void Game::initializeItems()
{
    for (int index = 0; index < MAX_ITEMS; index = index + 1)
    {
        items[index].active = false;
        items[index].special = false;
        items[index].name = "";
    }

    // Place a single key in the boss room (Room 6, bottom-left)
    items[0].position.x = 10;
    items[0].position.y = 22;
    items[0].symbol = '$';
    items[0].active = true;
    items[0].name = "Llave";
    items[0].special = true;

    // Disable the other legacy key slots (not used)
    items[1].active = false;
    items[2].active = false;
    items[3].active = false;
    items[4].active = false;

    items[5].position.x = 30;
    items[5].position.y = 10;
    items[5].symbol = 'P';
    items[5].active = true;
    items[5].name = "Pocion";
    items[5].special = false;

    items[6].position.x = 20;
    items[6].position.y = 38;
    items[6].symbol = 'P';
    items[6].active = true;
    items[6].name = "Pocion";
    items[6].special = false;

    items[7].position.x = 4;
    items[7].position.y = 4;
    items[7].symbol = 'S';
    items[7].active = true;
    items[7].name = "Espada";
    items[7].special = true;

    items[8].position.x = 45;
    items[8].position.y = 40;
    items[8].symbol = 'T';
    items[8].active = true;
    items[8].name = "Libretta";
    items[8].special = true;

    items[9].position.x = 34;
    items[9].position.y = 24;
    items[9].symbol = 'P';
    items[9].active = true;
    items[9].name = "Pocion";
    items[9].special = false;
}

void Game::updateFog()
{
    map.revealArea(player.position.x, player.position.y, known);
}

void Game::updateCurrentRoom()
{
    int previous = currentRoom;
    int x = player.position.x;
    int y = player.position.y;

    // New layout:
    // Top row (y 2..14): Room1 (left), Room2 (center), Room3 (right)
    // Bottom row (y 17..29): Room6 (left, boss), Room5 (center), Room4 (right)
    if (y >= 2 && y <= 14)
    {
        if (x >= 2 && x <= 24) currentRoom = 1;
        else if (x >= 27 && x <= 49) currentRoom = 2;
        else if (x >= 52 && x <= 74) currentRoom = 3;
    }
    else if (y >= 17 && y <= 29)
    {
        if (x >= 52 && x <= 74) currentRoom = 4;
        else if (x >= 27 && x <= 49) currentRoom = 5;
        else if (x >= 2 && x <= 24) currentRoom = 6;
    }

    // When entering Room 3 (top-right), open the door downward to Room 4
    if (currentRoom == 3 && previous != 3)
    {
        map.openDoor(63, 15);
    }

    if (currentRoom != previous)
    {
        char buf[128];
        sprintf(buf, "Has entrado en la Sala %d.", currentRoom);
        setStatus(buf);
    }
}

void Game::draw()
{
    // Build an offscreen char and attribute buffer then write atomically
    int minimapW = 18;
    int topTitleRows = 4; // dialogue + title + separator

    // clear buffers
    for (int i = 0; i < screenWidth * screenHeight; i = i + 1)
    {
        screenChars[i] = ' ';
        screenAttrs[i] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    // Dialogue (row 0-1)
    const char* line1 = "=== Diario de la Libreta Perdida ===";
    const char* line2a = "Tu libreta menciona una espada antigua que puede derrotar al Boss.";
    const char* line2b = "La espada brilla. El Boss final ya no es invencible.";
    int row = 0;
    int col = 0;
    for (int i = 0; line1[i] != '\0' && col + i < screenWidth; i = i + 1) screenChars[row * screenWidth + (col + i)] = line1[i];
    row = 1; col = 0;
    const char* chosen = player.hasSword ? line2b : line2a;
    for (int i = 0; chosen[i] != '\0' && col + i < screenWidth; i = i + 1) screenChars[row * screenWidth + (col + i)] = chosen[i];

    // Room title (row 2) and separator (row 3)
    char titleBuf[64];
    sprintf(titleBuf, "---- SALA %d ----", currentRoom);
    int titleLen = (int)strlen(titleBuf);
    int titleCol = (screenWidth / 2) - (titleLen / 2);
    if (titleCol < 0) titleCol = 0;
    row = 2; col = titleCol;
    for (int i = 0; titleBuf[i] != '\0' && col + i < screenWidth; i = i + 1) screenChars[row * screenWidth + (col + i)] = titleBuf[i];
    // separator
    row = 3;
    for (int c = 0; c < MAP_WIDTH && c < screenWidth; c = c + 1) screenChars[row * screenWidth + c] = '-';

    // Map area
    char buffer[MAP_HEIGHT][MAP_WIDTH];
    map.copyTo(buffer);

    for (int index = 0; index < MAX_ITEMS; index = index + 1)
    {
        if (items[index].active && canSee(items[index].position.x, items[index].position.y))
        {
            buffer[items[index].position.y][items[index].position.x] = items[index].symbol;
        }
    }

    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        Entity* enemyPtr = &enemies[index];
        if (enemyPtr->active && canSee(enemyPtr->position.x, enemyPtr->position.y))
        {
            buffer[enemyPtr->position.y][enemyPtr->position.x] = enemyPtr->symbol;
            if (enemyPtr->isDouble && enemyPtr->position.x + 1 < MAP_WIDTH)
            {
                buffer[enemyPtr->position.y][enemyPtr->position.x + 1] = enemyPtr->symbol;
            }
        }
    }

    buffer[player.position.y][player.position.x] = '@';

    // draw map into buffers
    int mapStartRow = topTitleRows;
    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            int sx = x;
            int sy = mapStartRow + y;
            char toPrint = known[y][x] ? buffer[y][x] : '?';
            WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            if (toPrint == '@') color = FOREGROUND_RED | FOREGROUND_INTENSITY;
            else if (toPrint == '!' || toPrint == 'B') color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            else if (toPrint == 'S' || toPrint == 'T' || toPrint == 'P') color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // pinkish
            else if (toPrint == '$') color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            else if (toPrint == '(' || toPrint == ')') color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            else if (toPrint == '#') color = FOREGROUND_RED | FOREGROUND_GREEN;
            screenChars[sy * screenWidth + sx] = toPrint;
            screenAttrs[sy * screenWidth + sx] = color;
        }

        // minimap at right
        int mxCol = MAP_WIDTH + 2;
        for (int mx = 0; mx < minimapW && mxCol + mx < screenWidth; mx = mx + 1)
        {
            int sampleX = (mx * MAP_WIDTH) / minimapW;
            char mt = known[y][sampleX] ? map.tileAt(sampleX, y) : ' ';
            WORD mcolor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            if (mt == '#') mcolor = FOREGROUND_RED | FOREGROUND_GREEN;
            else if (mt == 'X') mcolor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            int sx = mxCol + mx;
            int sy = mapStartRow + y;
            screenChars[sy * screenWidth + sx] = mt;
            screenAttrs[sy * screenWidth + sx] = mcolor;
        }
    }

    // Estado y controles debajo del mapa
    int panelRow = mapStartRow + MAP_HEIGHT;
    const char* statusPanel[] = {
        "+----------------------------------------------------------+",
        "| ESTADO DEL JUGADOR                                       |",
        "| VIDA    : 10                                             |",
        "| ESPADA  : No                                             |",
        "| OBJETO  : vacio                                          |",
        "| LIBRETA : Faltante                                       |",
        "+----------------------------------------------------------+"
    };
    const char* instructionPanel[] = {
        "+----------------------------------------------------------+",
        "| INSTRUCCIONES DE JUEGO                                    |",
        "| W/A/S/D   - Mover                                         |",
        "| E         - Recoger / usar item                           |",
        "| G         - Guardar partida                               |",
        "| L         - Cargar partida                                |",
        "| Q         - Salir                                         |",
        "| S  (Espada)  - Permite derrotar enemigos cercanos        |",
        "| $  (Llave)   - Recoge la llave para abrir la puerta ()    |",
        "| T  (Libretta)- Objetivo final                             |",
        "| P  (Pocion)  - Recupera 3 puntos de vida                   |",
        "| Busca la llave en la sala anterior para avanzar           |",
        "| Todas las 6 salas están visibles en la misma pantalla     |",
        "+----------------------------------------------------------+"
    };
    int panelWidth = (int)strlen(statusPanel[0]);
    int panelPad = (screenWidth - panelWidth) / 2;
    if (panelPad < 0) panelPad = 0;

    for (int i = 0; i < 7; i = i + 1)
    {
        int r = panelRow + i;
        int start = panelPad;
        for (int j = 0; statusPanel[i][j] != '\0' && start + j < screenWidth; j = j + 1)
        {
            screenChars[r * screenWidth + (start + j)] = statusPanel[i][j];
        }
    }

    for (int i = 0; i < 14; i = i + 1)
    {
        int r = panelRow + 7 + i;
        int start = panelPad;
        for (int j = 0; instructionPanel[i][j] != '\0' && start + j < screenWidth; j = j + 1)
        {
            screenChars[r * screenWidth + (start + j)] = instructionPanel[i][j];
        }
    }

    // update dynamic values inside status panel
    char itemName[16];
    if (player.hasItem)
    {
        if (player.carriedItem == '$')
        {
            strcpy(itemName, "Llave");
        }
        else if (player.carriedItem == 'P')
        {
            strcpy(itemName, "Pocion");
        }
        else
        {
            strcpy(itemName, "Objeto");
        }
    }
    else
    {
        strcpy(itemName, "vacio");
    }
    char swordText[8];
    strcpy(swordText, player.hasSword ? "Si" : "No");
    char treasureText[16];
    strcpy(treasureText, player.hasTreasure ? "Recogida" : "Faltante");

    char lineBuf[80];
    sprintf(lineBuf, "| VIDA    : %d", player.hp);
    int offset = panelPad + 2;
    for (int i = 0; lineBuf[i] != '\0' && offset + i < panelPad + panelWidth - 2; i = i + 1)
    {
        screenChars[(panelRow + 2) * screenWidth + (offset + i)] = lineBuf[i];
    }
    sprintf(lineBuf, "| ESPADA  : %s", swordText);
    for (int i = 0; lineBuf[i] != '\0' && offset + i < panelPad + panelWidth - 2; i = i + 1)
    {
        screenChars[(panelRow + 3) * screenWidth + (offset + i)] = lineBuf[i];
    }
    sprintf(lineBuf, "| OBJETO  : %s", itemName);
    for (int i = 0; lineBuf[i] != '\0' && offset + i < panelPad + panelWidth - 2; i = i + 1)
    {
        screenChars[(panelRow + 4) * screenWidth + (offset + i)] = lineBuf[i];
    }
    sprintf(lineBuf, "| LIBRETA : %s", treasureText);
    for (int i = 0; lineBuf[i] != '\0' && offset + i < panelPad + panelWidth - 2; i = i + 1)
    {
        screenChars[(panelRow + 5) * screenWidth + (offset + i)] = lineBuf[i];
    }

    // finally write buffers to console
    COORD writeCoord;
    writeCoord.X = 0;
    writeCoord.Y = 0;
    DWORD written = 0;
    WriteConsoleOutputCharacterA(consoleHandle, screenChars.data(), (DWORD)(screenChars.size()), writeCoord, &written);
    WriteConsoleOutputAttribute(consoleHandle, screenAttrs.data(), (DWORD)(screenAttrs.size()), writeCoord, &written);
}

void Game::drawMap()
{
    char buffer[MAP_HEIGHT][MAP_WIDTH];
    map.copyTo(buffer);

    for (int index = 0; index < MAX_ITEMS; index = index + 1)
    {
        if (items[index].active && canSee(items[index].position.x, items[index].position.y))
        {
            buffer[items[index].position.y][items[index].position.x] = items[index].symbol;
        }
    }

    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        Entity* enemyPtr = &enemies[index];
        if (enemyPtr->active && canSee(enemyPtr->position.x, enemyPtr->position.y))
        {
            buffer[enemyPtr->position.y][enemyPtr->position.x] = enemyPtr->symbol;
        }
    }

    buffer[player.position.y][player.position.x] = '@';

    const int minimapWidth = 18;
    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            char toPrint = known[y][x] ? buffer[y][x] : '?';
            WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            if (toPrint == '@') color = FOREGROUND_RED | FOREGROUND_INTENSITY;
            else if (toPrint == '!' || toPrint == 'B') color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            else if (toPrint == 'S' || toPrint == 'T' || toPrint == 'P') color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // pinkish
            else if (toPrint == '$') color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            else if (toPrint == '(' || toPrint == ')') color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            else if (toPrint == '#') color = FOREGROUND_RED | FOREGROUND_GREEN;
            SetConsoleTextAttribute(consoleHandle, color);
            std::cout << toPrint;
            SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        // draw minimap at right: sample across the row
        std::cout << "  ";
        for (int mx = 0; mx < minimapWidth; mx = mx + 1)
        {
            int sampleX = (mx * MAP_WIDTH) / minimapWidth;
            char mt = known[y][sampleX] ? map.tileAt(sampleX, y) : ' ';
            WORD mcolor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            if (mt == '#') mcolor = FOREGROUND_RED | FOREGROUND_GREEN;
            else if (mt == 'X') mcolor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            SetConsoleTextAttribute(consoleHandle, mcolor);
            std::cout << mt;
            SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        std::cout << '\n';
    }
}

void Game::drawRoomTitle()
{
    // Print room title centered
    const char* title = "SALA ";
    char buf[64];
    sprintf(buf, "---- %s%d ----", title, currentRoom);
    int pad = (MAP_WIDTH / 2) - (int)strlen(buf) / 2;
    if (pad < 0) pad = 0;
    for (int i = 0; i < pad; i = i + 1) std::cout << ' ';
    std::cout << buf << '\n';
    for (int i = 0; i < MAP_WIDTH; i = i + 1) std::cout << '-';
    std::cout << "\n";
}

void Game::drawControls()
{
    const char* lines[] = {
        "---- CONTROLES ----",
        "| Tecla | Accion               |",
        "|-------|----------------------|",
        "|   W   | Mover arriba         |",
        "|   A   | Mover izquierda      |",
        "|   S   | Mover abajo          |",
        "|   D   | Mover derecha       |",
        "|   E   | Guardar partida      |",
        "|   L   | Cargar partida       |",
        "|   P   | Recoger/usar item    |",
        "|   Q   | Salir                |"
    };
    int tableWidth = 24;
    int pad = (MAP_WIDTH / 2) - (tableWidth / 2);
    if (pad < 0) pad = 0;
    std::cout << "\n";
    for (int i = 0; i < 10; i = i + 1)
    {
        for (int s = 0; s < pad; s = s + 1) std::cout << ' ';
        std::cout << lines[i] << '\n';
    }
}

void Game::drawHUD()
{
    std::cout << "\nVida: " << player.hp << "  ";
    std::cout << "Espada: ";
    if (player.hasSword)
    {
        std::cout << "Si";
    }
    else
    {
        std::cout << "No";
    }
    std::cout << "  ";
    std::cout << "Objeto: ";
    if (player.hasItem)
    {
        std::cout << player.carriedItem;
    }
    else
    {
        std::cout << "(vacio)";
    }
    std::cout << "  ";
    std::cout << "Libretta: ";
    if (player.hasTreasure)
    {
        std::cout << "Recogida";
    }
    else
    {
        std::cout << "Faltante";
    }
    std::cout << "\n";
    std::cout << "Teclas: W/A/S/D mover, E recoger / usar, Q salir, S guardar.\n";
    std::cout << "Llaves verdes: $  Enemigos azules: ! y !!  Objetos rosa: S,T,P\n";
    std::cout << status << "\n";
}

void Game::drawMinimap()
{
    std::cout << "\nMini mapa del dungeon:\n";
    for (int y = 0; y < MAP_HEIGHT; y = y + 2)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 2)
        {
            if (known[y][x] == false)
            {
                std::cout << ' ';
            }
            else if (player.position.x == x && player.position.y == y)
            {
                std::cout << '@';
            }
            else
            {
                char tile = map.tileAt(x, y);
                if (tile == '#')
                {
                    std::cout << '#';
                }
                else if (tile == 'X')
                {
                    std::cout << 'X';
                }
                else
                {
                    std::cout << '.';
                }
            }
        }
        std::cout << '\n';
    }
}

void Game::drawDialogue()
{
    std::cout << "=== Diario de la Libreta Perdida ===\n";
    if (player.hasSword == false)
    {
        std::cout << "Tu libreta menciona una espada antigua que puede derrotar al Boss.\n";
    }
    else
    {
        std::cout << "La espada brilla. El Boss final ya no es invencible.\n";
    }
}

void Game::showMenu()
{
    std::system("cls");
    std::cout << "=================== DUNGEON CRAWLER RETRO ===================\n";
    std::cout << "1. Nueva partida\n";
    std::cout << "2. Cargar partida\n";
    std::cout << "3. Instrucciones\n";
    std::cout << "4. Salir\n";
    std::cout << "Elige una opción: ";

    char choice = '0';
    std::cin >> choice;
    std::cin.ignore(256, '\n');

    if (choice == '1')
    {
        inGame = true;
        loadRequested = false;
    }
    else if (choice == '2')
    {
        inGame = true;
        loadRequested = true;
    }
    else if (choice == '3')
    {
        showInstructions();
        inGame = false;
    }
    else
    {
        inGame = false;
    }
}

void Game::showInstructions()
{
    std::system("cls");
    std::cout << "Instrucciones:\n";
    std::cout << "- Usa W/A/S/D para moverte en tiempo real.\n";
    std::cout << "- Presiona P para recoger objetos cuando estés sobre ellos.\n";
    std::cout << "- Presiona E para guardar la partida.\n";
    std::cout << "- Presiona L para cargar la partida desde el archivo.\n";
    std::cout << "- Presiona Q para abandonar la partida.\n";
    std::cout << "- Sólo ves las casillas cercanas gracias a la niebla de guerra.\n";
    std::cout << "- Encuentra la espada para vencer al Boss final.\n";
    std::cout << "Presiona cualquier tecla para volver al menú...\n";
    while (_kbhit())
    {
        _getch();
    }
    _getch();
}

void Game::processInput()
{
    if (_kbhit() == 0)
    {
        return;
    }

    int input = _getch();
    if (input == 0 || input == 224)
    {
        int arrow = _getch();
        if (arrow == 72)
        {
            movePlayer(0, -1);
        }
        else if (arrow == 80)
        {
            movePlayer(0, 1);
        }
        else if (arrow == 75)
        {
            movePlayer(-1, 0);
        }
        else if (arrow == 77)
        {
            movePlayer(1, 0);
        }
        return;
    }

    if (input == 'w' || input == 'W')
    {
        movePlayer(0, -1);
    }
    else if (input == 's' || input == 'S')
    {
        movePlayer(0, 1);
    }
    else if (input == 'a' || input == 'A')
    {
        movePlayer(-1, 0);
    }
    else if (input == 'd' || input == 'D')
    {
        movePlayer(1, 0);
    }
    else if (input == 'e' || input == 'E')
    {
        checkItemPickup();
    }
    else if (input == 'g' || input == 'G')
    {
        saveGame();
    }
    else if (input == 'l' || input == 'L')
    {
        loadGame();
    }
    else if (input == 'q' || input == 'Q')
    {
        running = false;
    }
}

void Game::movePlayer(int dx, int dy)
{
    Position next;
    next.x = player.position.x + dx;
    next.y = player.position.y + dy;

    if (map.isWalkable(next.x, next.y) == false)
    {
        setStatus("No puedes atravesar paredes. Busca otro camino.");
        return;
    }

    lastPlayerPos = player.position;
    player.position = next;
    needRedraw = true;
    updateFog();
    updateCurrentRoom();
}

void Game::updateEnemies()
{
    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        Entity* enemyPtr = &enemies[index];
        if (enemyPtr->active == false)
        {
            continue;
        }
        int speed = 4;
        if (enemyPtr->type == EnemyType::Stalker)
        {
            speed = 5;
        }
        else if (enemyPtr->type == EnemyType::Boss)
        {
            speed = 6;
        }
        if (enemyTick % speed == 0)
        {
            updateEnemy(enemyPtr);
        }
    }
}

void Game::updateEnemy(Entity* enemy)
{
    Position current = enemy->position;
    Position target = player.position;
    Position next = current;

    if (enemy->type == EnemyType::Chaser)
    {
        if (target.x < current.x)
        {
            next.x = current.x - 1;
        }
        else if (target.x > current.x)
        {
            next.x = current.x + 1;
        }
        else if (target.y < current.y)
        {
            next.y = current.y - 1;
        }
        else if (target.y > current.y)
        {
            next.y = current.y + 1;
        }
    }
    else if (enemy->type == EnemyType::Stalker)
    {
        if ((current.x + current.y) % 2 == 0)
        {
            if (target.y < current.y)
            {
                next.y = current.y - 1;
            }
            else if (target.y > current.y)
            {
                next.y = current.y + 1;
            }
        }
        else
        {
            if (target.x < current.x)
            {
                next.x = current.x - 1;
            }
            else if (target.x > current.x)
            {
                next.x = current.x + 1;
            }
        }
    }
    else if (enemy->type == EnemyType::Shadow)
    {
        next = lastPlayerPos;
    }
    else if (enemy->type == EnemyType::Boss)
    {
        int distanceX = target.x - current.x;
        int distanceY = target.y - current.y;
        if (distanceX < 0)
        {
            next.x = current.x - 1;
        }
        else if (distanceX > 0)
        {
            next.x = current.x + 1;
        }
        if (distanceY < 0)
        {
            next.y = current.y - 1;
        }
        else if (distanceY > 0)
        {
            next.y = current.y + 1;
        }
    }

    bool canMove = map.isWalkable(next.x, next.y);
    if (enemy->isDouble)
    {
        canMove = canMove && map.isWalkable(next.x + 1, next.y);
    }
    if (canMove && (next.x != player.position.x || next.y != player.position.y || enemy->type == EnemyType::Boss))
    {
        if (enemy->position.x != next.x || enemy->position.y != next.y)
        {
            enemy->position = next;
            needRedraw = true;
        }
    }
}

void Game::checkCollisions()
{
    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        Entity* enemyPtr = &enemies[index];
        if (enemyPtr->active == false)
        {
            continue;
        }
        if (enemyPtr->position.x == player.position.x && enemyPtr->position.y == player.position.y)
        {
            if (enemyPtr->isBoss)
            {
                if (player.hasSword)
                {
                    enemyPtr->active = false;
                    setStatus("Derrotaste al Boss final con tu espada.");
                    player.hasTreasure = true;
                    return;
                }
                else
                {
                    player.hp = player.hp - 3;
                    setStatus("Boss final: necesitas la espada para sobrevivir.");
                    beepLong();
                    if (player.hp <= 0)
                    {
                        defeat = true;
                        running = false;
                    }
                    return;
                }
            }
            else
            {
                if (player.hasSword)
                {
                    enemyPtr->active = false;
                    setStatus("Mataste al enemigo con tu espada. Sigue adelante.");
                    beepShort();
                    return;
                }
                player.hp = player.hp - 1;
                setStatus("Un enemigo te golpeó. Mantente alerta.");
                beepShort();
                if (player.hp <= 0)
                {
                    defeat = true;
                    running = false;
                }
                return;
            }
        }
    }

    if (map.tileAt(player.position.x, player.position.y) == 'X')
    {
        if (player.hasTreasure == true)
        {
            victory = true;
            running = false;
        }
        else
        {
            setStatus("Debes traer la libreta antes de escapar por el portal.");
        }
    }
}

void Game::checkItemPickup()
{
    bool found = false;
    for (int index = 0; index < MAX_ITEMS; index = index + 1)
    {
        Item* itemPtr = &items[index];
        if (itemPtr->active && itemPtr->position.x == player.position.x && itemPtr->position.y == player.position.y)
        {
            found = true;
            itemPtr->active = false;
            if (itemPtr->symbol == 'S')
            {
                player.hasSword = true;
                setStatus("Has recogido la espada. Ahora puedes derrotar enemigos cercanos.");
                beepShort();
                killNearbyEnemies();
                return;
            }
            if (itemPtr->symbol == 'T')
            {
                player.hasTreasure = true;
                setStatus("Libretta recogida. Ve al portal con cuidado.");
                beepShort();
                return;
            }
            if (itemPtr->symbol == '$')
            {
                player.hasItem = true;
                player.carriedItem = itemPtr->symbol;
                setStatus("Recogiste la llave.");
                beepShort();
                return;
            }
            if (itemPtr->symbol == 'P')
            {
                player.hp = player.hp + 3;
                setStatus("Pocion bebida. Recuperaste vida.");
                beepShort();
                return;
            }
        }
    }

    if (found == false)
    {
        if (player.hasSword)
        {
            int killed = 0;
            for (int index = 0; index < MAX_ENEMIES; index = index + 1)
            {
                Entity* enemyPtr = &enemies[index];
                if (enemyPtr->active == false || enemyPtr->isBoss)
                {
                    continue;
                }
                int dx = enemyPtr->position.x - player.position.x;
                int dy = enemyPtr->position.y - player.position.y;
                if ((dx >= -1 && dx <= 1) && (dy >= -1 && dy <= 1))
                {
                    enemyPtr->active = false;
                    killed = killed + 1;
                }
            }
            if (killed > 0)
            {
                setStatus("Atacaste con la espada y mataste a los enemigos cercanos.");
                beepShort();
                return;
            }
        }
        setStatus("No hay nada que recoger aqui.");
    }
}

void Game::killNearbyEnemies()
{
    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        Entity* enemyPtr = &enemies[index];
        if (enemyPtr->active == false || enemyPtr->isBoss)
        {
            continue;
        }
        int dx = enemyPtr->position.x - player.position.x;
        int dy = enemyPtr->position.y - player.position.y;
        if ((dx >= -1 && dx <= 1) && (dy >= -1 && dy <= 1))
        {
            enemyPtr->active = false;
        }
    }
}

void Game::saveGame()
{
    std::ofstream file("savegame.txt");
    if (file.is_open() == false)
    {
        setStatus("Error guardando la partida.");
        return;
    }

    file << player.position.x << ' ' << player.position.y << ' ' << player.hp << ' ' << player.hasSword << ' ' << player.hasTreasure << ' ' << player.hasItem << ' ' << int(player.carriedItem) << '\n';
    for (int index = 0; index < MAX_ITEMS; index = index + 1)
    {
        file << items[index].symbol << ' ' << items[index].position.x << ' ' << items[index].position.y << ' ' << items[index].active << '\n';
    }
    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        file << int(enemies[index].type) << ' ' << enemies[index].symbol << ' ' << enemies[index].position.x << ' ' << enemies[index].position.y << ' ' << enemies[index].hp << ' ' << enemies[index].active << ' ' << enemies[index].isBoss << '\n';
    }
    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            file << (known[y][x] ? '1' : '0');
        }
        file << '\n';
    }
    file.close();
    setStatus("Partida guardada en savegame.txt.");
}

bool Game::loadGame()
{
    std::ifstream file("savegame.txt");
    if (file.is_open() == false)
    {
        setStatus("No se encontró savegame.txt.");
        return false;
    }

    map.build();
    initializeFog();

    int swordValue;
    int treasureValue;
    int itemValue;
    int carriedValue;
    file >> player.position.x >> player.position.y >> player.hp >> swordValue >> treasureValue >> itemValue >> carriedValue;
    player.hasSword = swordValue != 0;
    player.hasTreasure = treasureValue != 0;
    player.hasItem = itemValue != 0;
    player.carriedItem = char(carriedValue);

    for (int index = 0; index < MAX_ITEMS; index = index + 1)
    {
        int activeValue;
        file >> items[index].symbol >> items[index].position.x >> items[index].position.y >> activeValue;
        items[index].active = activeValue != 0;
    }

    for (int index = 0; index < MAX_ENEMIES; index = index + 1)
    {
        int typeValue;
        int activeValue;
        int isBossValue;
        file >> typeValue >> enemies[index].symbol >> enemies[index].position.x >> enemies[index].position.y >> enemies[index].hp >> activeValue >> isBossValue;
        enemies[index].type = EnemyType(typeValue);
        enemies[index].active = activeValue != 0;
        enemies[index].isBoss = isBossValue != 0;
    }

    for (int y = 0; y < MAP_HEIGHT; y = y + 1)
    {
        char row[MAP_WIDTH + 2];
        file >> row;
        for (int x = 0; x < MAP_WIDTH; x = x + 1)
        {
            known[y][x] = (row[x] == '1');
        }
    }

    lastPlayerPos = player.position;
    running = true;
    return true;
}

void Game::setStatus(const char* message)
{
    std::strncpy(status, message, sizeof(status) - 1);
    status[sizeof(status) - 1] = '\0';
}

void Game::beepShort()
{
    Beep(800, 90);
}

void Game::beepLong()
{
    Beep(500, 220);
}

bool Game::canSee(int x, int y) const
{
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
    {
        return false;
    }
    return known[y][x];
}
