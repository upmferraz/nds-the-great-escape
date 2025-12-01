#include "Map.h"
#include "MapData.h"
#include "Utils.h"

// VALORES SINTONIZADOS
#define MAP_OFFSET 3240
#define MAP_WIDTH  54   

Map::Map() {
    mapOffset = MAP_OFFSET;
    mapWidth = MAP_WIDTH;
}

const u8* getTileData(u8 index) {
    // Seleciona o banco de gráficos correto
    if (index < 45) {
        if (index * 8 < exterior_tiles_1_size) return &exterior_tiles_1[index * 8];
    }
    else if (index < 139) {
        if (index * 8 < exterior_tiles_2_size) return &exterior_tiles_2[index * 8];
    }
    else if (index >= 204) {
        if (index * 8 < exterior_tiles_2_size) return &exterior_tiles_2[index * 8];
    }
    else {
        if (index * 8 < exterior_tiles_3_size) return &exterior_tiles_3[index * 8];
    }
    return 0;
}

void Map::drawTile8x8(u16* buffer, int screenX, int screenY, u8 tileIndex) {
    const u8* tileData = getTileData(tileIndex);
    if (!tileData) return;

    for (int y = 0; y < 8; y++) {
        int py = screenY + y;
        if (py < 0 || py >= 192) continue;

        u8 row = tileData[y];
        for (int x = 0; x < 8; x++) {
            int px = screenX + x;
            if (px < 0 || px >= 256) continue;
            
            // Desenha pixel branco (tinta)
            if ((row >> (7 - x)) & 1) {
                buffer[px + py * 256] = CLR_WHITE;
            }
        }
    }
}

void Map::drawSuperTile(u16* buffer, int screenX, int screenY, u8 superTileIndex) {
    // Filtra o chão (255) para não desenhar nada (transparente/preto)
    // Se quiseres ver o chão a cinzento para debug, podes descomentar o código do passo anterior
    if (superTileIndex == 0xFF) return; 

    // Proteção de limites
    if (superTileIndex * 16 >= super_tiles_raw_size) return;

    const u8* defs = &super_tiles_raw[superTileIndex * 16];

    for (int ty = 0; ty < 4; ty++) {
        for (int tx = 0; tx < 4; tx++) {
            u8 tileIndex = defs[tx + (ty * 4)];
            drawTile8x8(buffer, screenX + (tx * 8), screenY + (ty * 8), tileIndex);
        }
    }
}

void Map::draw(u16* buffer, int camX, int camY) {
    // Desenha uma área ligeiramente maior que o ecrã para evitar "pop-in"
    for (int y = 0; y < 8; y++) { 
        for (int x = 0; x < 9; x++) { 
            int sx = (x * 32) - (camX % 32);
            int sy = (y * 32) - (camY % 32);
            
            int worldSTX = (camX / 32) + x;
            int worldSTY = (camY / 32) + y;

            // Usa a largura sintonizada (54)
            if (worldSTX < 0 || worldSTX >= this->mapWidth) continue;

            int layoutIndex = this->mapOffset + worldSTX + (worldSTY * this->mapWidth);
            
            if (layoutIndex >= 0 && layoutIndex < super_tiles_raw_size) {
                u8 stIndex = super_tiles_raw[layoutIndex];
                drawSuperTile(buffer, sx, sy, stIndex);
            }
        }
    }
}
