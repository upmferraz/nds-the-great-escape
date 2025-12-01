#include <nds.h>
#include <stdio.h>
#include "GameWrapper.h"
#include "Tiles.h"
#include "Map.h"
#include "Utils.h"

#ifndef CLR_RED
#define CLR_RED RGB15(31,0,0)
#endif

Map gameMap;
int animFrame = 0;
int animTimer = 0;
bool isMoving = false;

int camX = 0;
int camY = 0;

GameWrapper::GameWrapper() {
    playerX = 128; 
    playerY = 128;
    vram_buffer = VRAM_A;
}

GameWrapper::~GameWrapper() {}

void GameWrapper::init() {
    videoSetMode(MODE_FB0);
    vramSetBankA(VRAM_A_LCD);
    consoleDemoInit(); 
    vram_buffer = VRAM_A;
}

void GameWrapper::update() {
    scanKeys();
    u16 keys = keysHeld();
    u16 kDown = keysDown();
    isMoving = false;
    
    // Movimento
    if (keys & KEY_UP)    { playerY--; isMoving = true; }
    if (keys & KEY_DOWN)  { playerY++; isMoving = true; }
    if (keys & KEY_LEFT)  { playerX--; isMoving = true; }
    if (keys & KEY_RIGHT) { playerX++; isMoving = true; }

    // --- DEBUG AVANÇADO ---
    // Ajuste Fino de Offset
    if (kDown & KEY_L) gameMap.mapOffset -= 1;
    if (kDown & KEY_R) gameMap.mapOffset += 1;
    
    // Ajuste de Linhas (Salta 54 bytes - a largura do mapa)
    // Usa isto para encontrar o topo do mapa!
    if (kDown & KEY_X) gameMap.mapOffset += 54;
    if (kDown & KEY_Y) gameMap.mapOffset -= 54;

    // Ajuste de Largura
    if (kDown & KEY_A) gameMap.mapWidth++;
    if (kDown & KEY_B) gameMap.mapWidth--;
    
    iprintf("\x1b[0;0HOff:%d W:%d   ", gameMap.mapOffset, gameMap.mapWidth);
    iprintf("\n\nL/R: Off +/- 1");
    iprintf("\nY/X: Off +/- 54 (Linhas)");
    iprintf("\nB/A: Width +/- 1");
    // ----------------------
    
    camX = playerX - (256 / 2);
    camY = playerY - (192 / 2);

    if (isMoving) {
        animTimer++;
        if (animTimer > 5) { 
            animFrame = (animFrame + 1) % 4;
            animTimer = 0;
        }
    } else {
        animFrame = 0;
    }
}

void GameWrapper::drawPixel(int x, int y, u16 color) {
    if (x >= 0 && x < 256 && y >= 0 && y < 192) {
        vram_buffer[x + y * 256] = color;
    }
}

void GameWrapper::drawSpectrumSprite(int x, int y, const u8* data, int height, int widthBytes) {
    int screenX = x - camX;
    int screenY = y - camY;
    if (screenX + (widthBytes*8) < 0 || screenX >= 256 || screenY + height < 0 || screenY >= 192) return;

    for (int row = 0; row < height; row++) {
        for (int byteCol = 0; byteCol < widthBytes; byteCol++) {
            u8 byte = data[row * widthBytes + byteCol];
            for (int bit = 0; bit < 8; bit++) {
                if ((byte >> (7 - bit)) & 1) {
                    drawPixel(screenX + (byteCol * 8) + bit, screenY + row, CLR_WHITE);
                }
            }
        }
    }
}

void GameWrapper::render() {
    if (!vram_buffer) return;
    for(int i=0; i<256*192; i++) vram_buffer[i] = CLR_BLACK;
    gameMap.draw(vram_buffer, camX, camY);
    drawSpectrumSprite(playerX, playerY, prisoner_anim_away[animFrame], prisoner_height, 2);
}
