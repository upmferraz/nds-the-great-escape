#include <nds.h>
#include "GameWrapper.h"

int main(void) {
    consoleDemoInit(); // Mantemos a consola para debug se necessário
    irqSet(IRQ_VBLANK, 0);
    
    GameWrapper game;
    game.init();

    while(1) {
        game.update();
        swiIntrWait(1, IRQ_VBLANK);
        game.render();
    }
    return 0;
}
