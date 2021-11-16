import core.stdc.stdio;
import mgba;
import tonc.tonc_types;

enum REG_BASE = 0x04000000;
// __gshared vu16 REG_IFBIOS = *(cast(vu16*) REG_BASE - 0x0008);
ref vu16 REG_IFBIOS() { return *(cast(vu16*) REG_BASE - 0x0008); }

extern(C) int main() {
	// nothing
	printf("Hello, world!\n");

	if (mgba_open()) {
		mgba_printf(MGBA_LOG_LEVEL.MGBA_LOG_ERROR, "Hello, world, %s!\n", cast(const char*)"Bean Machine");
	}

	return 0;
}
