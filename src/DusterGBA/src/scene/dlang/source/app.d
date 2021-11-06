extern(C): // disable D mangling

// D imports


// D fake imports
int min(int a, int b) {
  return a < b ? a : b;
}

int max(int a, int b) {
  return a > b ? a : b;
}


// Required module entrypoint
void _start() { }


// External imports
int bean_keydown();
void bean_printi(int x);


private int vel = 0;
private int pos = 0;

void bean_start() {
  //
}

void bean_update() {
  const auto curr = bean_keydown();

  // Accelerate upwards if pressed, downwards if not pressed
  // Cap velocity at 50 units/frame
  vel += curr ? 1 : -1;
  vel = max(-50, min(50, vel));

  // Apply velocity to position
  // Limit position to between 0 and 10000 units
  pos += vel;
  const auto prepos = pos;
  pos = max(0, min(10_000, pos));

  // Zero velocity if position hits limit
  if (pos != prepos) {
    vel = 0;
  }

  // Show position
  bean_printi(pos);
}
