import std.stdio;
import std.file;

import dustermap;

int main(string[] args) {
	writeln("duster-mapc map compiler");
	if (args.length < 3) {
		writeln("usage: duster-mapc <in_map> <out_bin>");
		return 1;
	}

	string in_fn = args[1];
	string ou_fn = args[2];

	DusterMap parsed_map = parse_map_file(in_fn);
	ubyte[] binmap = compile_duster_map(parsed_map);

	std.file.write(ou_fn, binmap);

	return 0;
}