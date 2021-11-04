with open("build/untouched.wasm", "rb") as f:
    raw = f.read()

hexes = ", ".join(hex(b) for b in raw)

header = f"""
unsigned char bean_wasm[] = {{
  {hexes}
}};

unsigned int bean_wasm_len = {len(raw)};
"""[1:]

with open("bean.wasm.h", "w") as f:
    f.write(header)
