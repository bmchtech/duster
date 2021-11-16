import core.stdc.stdio;
import mgba;
import tonc.tonc_types;

extern(C) int main() {
	// nothing
	printf("Hello, world!\n");

	if (mgba_open()) {
		mgba_printf(MGBA_LOG_LEVEL.MGBA_LOG_ERROR, "Hello, world, %s!\n", cast(const char*)"Bean Machine");
	}

	return 0;
}
