#include "GameWrapper.h"
#include "Tiles.h" 
#include <stdio.h>

// Cores
#define CLR_BLACK   RGB15(0,0,0)
#define CLR_WHITE   RGB15(31,31,31)

// Variáveis Globais de Animação (para este ficheiro apenas)
int animFrame = 0;
int animTimer = 0;
bool isMoving = false;

GameWrapper::GameWrapper() {
    playerX = 128;
    playerY = 96;
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
    
    if (keys & KEY_UP)    { playerY--; isMoving = true; }
    if (keys & KEY_DOWN)  { playerY++; isMoving = true; }
    if (keys & KEY_LEFT)  { playerX--; isMoving = true; }
    if (keys & KEY_RIGHT) { playerX++; isMoving = true; }
    
    // Limites
    if(playerX < 0) playerX = 0;
    if(playerY < 0) playerY = 0;
    if(playerX > 256 - 24) playerX = 256 - 24;
    if(playerY > 192 - 27) playerY = 192 - 27;

    // Animação
    if (isMoving) {
        animTimer++;
        if (animTimer > 5) { 
            animFrame++;
            if (animFrame > 3) animFrame = 0;
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
    // Tenta desenhar assumindo que os dados no array estão sequenciais:
    // [Linha 0 Byte 0] [Linha 0 Byte 1] [Linha 0 Byte 2]
    // [Linha 1 Byte 0] [Linha 1 Byte 1] [Linha 1 Byte 2]
    // ...
    
    for (int row = 0; row < height; row++) {
        for (int byteCol = 0; byteCol < widthBytes; byteCol++) {
            
            // O truque está aqui: Como acedemos ao índice.
            int byteIndex = (row * widthBytes) + byteCol;
            
            // PROTEÇÃO DE MEMÓRIA:
            // Se estivermos a ler lixo, desenha preto para vermos o limite
            u8 byte = data[byteIndex];
            
            for (int bit = 0; bit < 8; bit++) {
                int pixelX = x + (byteCol * 8) + bit;
                int pixelY = y + row;
                
                if ((byte >> (7 - bit)) & 1) {
                    drawPixel(pixelX, pixelY, CLR_WHITE);
                }
            }
        }
    }
}

void GameWrapper::render() {
    // Limpar ecrã (Preto)
    for(int i=0; i<256*192; i++) vram_buffer[i] = CLR_BLACK;

    // Desenhar Prisioneiro Animado
    drawSpectrumSprite(playerX, playerY, prisoner_anim_away[animFrame], prisoner_height, 2);
    // Desenhar Chão (Tiles)
    for(int i=0; i < 8; i++) {
        drawSpectrumSprite(20 + (i * 20), 150, &exterior_tiles_0[i*8], 8, 1);
    }
}
