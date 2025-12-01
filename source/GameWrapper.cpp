#include <nds.h>
#include <stdio.h>
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
    // Posição inicial (ajusta conforme necessário para não começares numa parede)
    playerX = 300; 
    playerY = 300;
    vram_buffer = VRAM_A;
}

GameWrapper::~GameWrapper() {}

void GameWrapper::init() {
    videoSetMode(MODE_FB0);
    vramSetBankA(VRAM_A_LCD);
    // consoleDemoInit(); // Descomenta se quiseres debug de texto
    vram_buffer = VRAM_A;
}

void GameWrapper::update() {
    scanKeys();
    u16 keys = keysHeld();
    isMoving = false;
    
    int speed = 2;

    // Controlos de Movimento
    if (keys & KEY_UP)    { playerY -= speed; isMoving = true; }
    if (keys & KEY_DOWN)  { playerY += speed; isMoving = true; }
    if (keys & KEY_LEFT)  { playerX -= speed; isMoving = true; }
    if (keys & KEY_RIGHT) { playerX += speed; isMoving = true; }
    
    // A câmara segue o jogador
    camX = playerX - (256 / 2);
    camY = playerY - (192 / 2);

    // Limites básicos da câmara
    if (camX < 0) camX = 0;
    if (camY < 0) camY = 0;

    // Animação do Sprite
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

    // 2. Mapa (Fundo)
    gameMap.draw(vram_buffer, camX, camY);

    // 3. Jogador (Frente)
    drawSpectrumSprite(playerX, playerY, prisoner_anim_away[animFrame], prisoner_height, 2);
}
