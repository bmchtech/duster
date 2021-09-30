#include "dusk.h"
#include "ds_sys.h"
#include "scenes.h"
#include "contrib/mgba.h"

int main() {
    dusk_init_all();

#ifdef DEBUG
    mgba_open();
#endif

    dusk_scene_set(logo_scene);

    while (TRUE) {
        key_poll(); // update inputF
        dusk_scene_update();
    }
}