#pragma once
#include <nds.h>

// Definições de cores para simular o Spectrum
#define CLR_BLACK   RGB15(0,0,0)
#define CLR_WHITE   RGB15(31,31,31)
#define CLR_RED     RGB15(31,0,0)    // <--- Adicionado
#define CLR_CYAN    RGB15(0,31,31)
#define CLR_MAGENTA RGB15(31,0,31)

// Helper para converter coordenadas 2D em índice de array
inline int getPixelIndex(int x, int y) {
    return x + (y * 256);
}
