import sys
import os

if len(sys.argv) != 3:
  print("Usage: python3 packer.py <input_file> <output_file>")

src = sys.argv[1]
dst = sys.argv[2]

with open(src, "rb") as f:
    raw = f.read()

hexes = ", ".join(hex(b) for b in raw)

header = f"""
unsigned char wasm_src[] = {{
  {hexes}
}};

unsigned int wasm_src_len = {len(raw)};
"""[1:]

with open(dst, "w") as f:
    f.write(header)
