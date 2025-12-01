#pragma once
#include <nds.h>

class Map {
public:
    Map();
    void draw(u16* buffer, int camX, int camY);
    
    // Variáveis de controlo manual
    int mapOffset;
    int mapWidth;

private:
    void drawTile8x8(u16* buffer, int screenX, int screenY, u8 tileIndex);
    void drawSuperTile(u16* buffer, int screenX, int screenY, u8 superTileIndex);
};
