#include <PicoDVI.h>

// Display config
DVIGFX16 display(DVI_RES_320x240p60, picodvi_dvi_cfg);

// Constants
const int PIXEL_SIZE = 10;//8;
const int GRID_WIDTH = 16;  // use constant, not expression
const int GRID_HEIGHT = 12;

enum TileType {
  EMPTY = 0,
  PLAYER = 1,
  GOAL = 2,
  OBSTACLE = 3
};

//uint8_t levelMap[GRID_HEIGHT][GRID_WIDTH] = {0};
uint8_t levelMap[12][16] = {
  {0, 2, 0, 0, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0},
  {0, 0, 0, 3, 0, 3, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0},
  {0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 2, 0, 0, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0},
  {3, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0},
  {3, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 2, 0},
  {3, 0, 0, 0, 0, 3, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
// Player position
int playerX = 2;
int playerY = 2;

void setup() {
  Serial.begin(115200);
  if (!display.begin()) {
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }

  // Set up map
  levelMap[playerY][playerX] = PLAYER;
  //levelMap[10][20] = GOAL;
  //levelMap[5][5] = OBSTACLE;
  //levelMap[5][6] = OBSTACLE;
  //levelMap[5][7] = OBSTACLE;

  drawFullMap();
  printLevelMap();
}

void loop() {
  if (Serial.available()) {
    char c = tolower(Serial.read());
    int dx = 0, dy = 0;

    if (c == 'w') dy = -1;
    if (c == 's') dy = 1;
    if (c == 'a') dx = -1;
    if (c == 'd') dx = 1;

    movePlayer(dx, dy);
  }

  delay(30);
}

// === Functions ===

const int Y_OFFSET = 30; // Shift map 20 pixels down
const int X_OFFSET = 60; // Shift map 20 pixels down
void drawTile(int x, int y, TileType tile) {
  uint16_t color = 0x0000;
  switch (tile) {
    case PLAYER: color = 0xF800; break;
    case GOAL: color = 0x001F; break;
    case OBSTACLE: color = 0x07E0; break;
    default: color = 0x0000; break;
  }
  display.fillRect(x * PIXEL_SIZE + X_OFFSET, y * PIXEL_SIZE + Y_OFFSET, PIXEL_SIZE, PIXEL_SIZE, color);
}


void drawFullMap() {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      drawTile(x, y, (TileType)levelMap[y][x]);
    }
  }
}

bool isWalkable(int x, int y) {
  return x >= 0 && x < GRID_WIDTH &&
         y >= 0 && y < GRID_HEIGHT &&
         levelMap[y][x] != OBSTACLE;
}

void movePlayer(int dx, int dy) {
  int newX = playerX + dx;
  int newY = playerY + dy;

  if (!isWalkable(newX, newY)) return;

  levelMap[playerY][playerX] = EMPTY;
  drawTile(playerX, playerY, EMPTY);

  playerX = newX;
  playerY = newY;

  if (levelMap[playerY][playerX] == GOAL) {
    Serial.println("🎉 Goal reached!");
  }

  levelMap[playerY][playerX] = PLAYER;
  drawTile(playerX, playerY, PLAYER);

  //printLevelMap();
}

void printLevelMap() {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      Serial.print(levelMap[y][x]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("--------------------");
}
