import core.stdc.stdio;
import tonc;
import dusk.contrib.mgba;
import logo;
import dusk;

extern(C) int main() {
	// nothing
	printf("Hello, world!\n");

	if (mgba_open()) {
		mgba_printf(MGBA_LOG_LEVEL.MGBA_LOG_ERROR, "Hello, world, %s!\n", cast(const char*)"Bean Machine");
	}

	// start logo scene
	dusk_init_all();

    dusk_scene_set(logo_scene);

    while (TRUE) {
        key_poll(); // update inputF
        dusk_scene_update();
    }

	return 0;
}
