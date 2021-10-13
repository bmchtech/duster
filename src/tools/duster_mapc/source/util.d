module util;

import std.utf;

public static char* c_str(string str) {
    return str.toUTFz!(char*)();
}