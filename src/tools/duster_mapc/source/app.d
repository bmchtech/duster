import std.stdio;
import std.file;

import cute_tiled;
import util;

int main(string[] args) {
	writeln("duster-mapc map compiler");
	if (args.length < 3) {
		writeln("usage: duster-mapc <in_map> <out_bin>");
		return 1;
	}

	string in_fn = args[1];
	string ou_fn = args[2];

	ubyte[] binmap = compile_map_file(in_fn);

	std.file.write(ou_fn, binmap);

	return 0;
}

ubyte[] compile_map_file(string map_file) {
	ubyte[] binmap;

	// load map
	cute_tiled_map_t* map = cute_tiled_load_map_from_file(map_file.c_str(), null);
	writefln("map size: %sx%s", map.width, map.height);

	return binmap;
}
