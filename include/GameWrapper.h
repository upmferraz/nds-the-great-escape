#pragma once
#include <nds.h>

class GameWrapper {
public:
    GameWrapper();
    ~GameWrapper();

    void init();
    void update();
    void render();
    
    // AGORA É PÚBLICA: Para que o RoomRenderer possa desenhar
    void drawSpectrumSprite(int x, int y, const u8* data, int height, int widthBytes);

private:
    u16* vram_buffer;
    int playerX, playerY;
    
    void drawPixel(int x, int y, u16 color);
};
