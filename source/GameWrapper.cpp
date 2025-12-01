#include <nds.h>
#include "GameWrapper.h"
#include "Tiles.h"
#include "Map.h"
#include "Utils.h"

Map gameMap;
int animFrame = 0;
int animTimer = 0;
bool isMoving = false;

// Posição da Câmara
int camX = 0;
int camY = 0;

GameWrapper::GameWrapper() {
    // Posição inicial (Ajustada para uma zona jogável)
    playerX = 200; 
    playerY = 200;
    vram_buffer = VRAM_A;
}

GameWrapper::~GameWrapper() {}

void GameWrapper::init() {
    videoSetMode(MODE_FB0);
    vramSetBankA(VRAM_A_LCD);
    vram_buffer = VRAM_A;
}

void GameWrapper::update() {
    scanKeys();
    u16 keys = keysHeld();
    isMoving = false;
    
    // Movimento
    if (keys & KEY_UP)    { playerY--; isMoving = true; }
    if (keys & KEY_DOWN)  { playerY++; isMoving = true; }
    if (keys & KEY_LEFT)  { playerX--; isMoving = true; }
    if (keys & KEY_RIGHT) { playerX++; isMoving = true; }
    
    // Câmara segue o jogador
    camX = playerX - (256 / 2);
    camY = playerY - (192 / 2);

    // Limites da Câmara (Opcional: impede ver números negativos do mapa)
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;

    // Animação
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

    // 1. Limpar
    for(int i=0; i<256*192; i++) vram_buffer[i] = CLR_BLACK;

    // 2. Mapa
    gameMap.draw(vram_buffer, camX, camY);

    // 3. Jogador
    drawSpectrumSprite(playerX, playerY, prisoner_anim_away[animFrame], prisoner_height, 2);
}
