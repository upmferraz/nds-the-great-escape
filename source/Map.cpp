#include "Map.h"
#include "MapData.h"
#include "Utils.h"

// Valores descobertos por ti
#define MAP_OFFSET 4060
#define MAP_WIDTH  54

Map::Map() {
    // Não precisamos de inicializar nada dinâmico agora
}

const u8* getTileData(u8 index) {
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
            
            // Desenha o pixel do tile (Branco)
            if ((row >> (7 - x)) & 1) {
                buffer[px + py * 256] = CLR_WHITE;
            }
        }
    }
}

void Map::drawSuperTile(u16* buffer, int screenX, int screenY, u8 superTileIndex) {
    // Proteção básica para não ler fora da RAM
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
    // Otimização: Desenhar apenas o necessário (9x7 SuperTiles cobrem o ecrã da DS)
    for (int y = 0; y < 7; y++) {
        for (int x = 0; x < 9; x++) {
            int sx = (x * 32) - (camX % 32);
            int sy = (y * 32) - (camY % 32);
            
            // Coordenada no mundo (em supertiles)
            int worldSTX = (camX / 32) + x;
            int worldSTY = (camY / 32) + y;

            // Fórmula Final com os teus valores
            int layoutIndex = MAP_OFFSET + worldSTX + (worldSTY * MAP_WIDTH);
            
            if (layoutIndex >= 0 && layoutIndex < super_tiles_raw_size) {
                u8 stIndex = super_tiles_raw[layoutIndex];
                drawSuperTile(buffer, sx, sy, stIndex);
            }
        }
    }
}
