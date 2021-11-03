#include "dusk.h"
#include "scene.h"

__attribute__((used)) const char* GAME_VERSION = "$DUSTER v0.2";


int main() {
    dusk_scene_set(bean_scene);

    while (TRUE) {
        key_poll(); // update inputF

        dusk_scene_update();
    }
}
