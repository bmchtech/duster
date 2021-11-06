// The entry file of your WebAssembly module.

@external("duster", "bean_printi")
declare function bean_printi(x: i32): void;

@external("duster", "bean_keydown")
declare function bean_keydown(): i32;

var vel = 0;
var pos = 0;

export function bean_start(): void {
  //
}

export function bean_update(): void {
  // Get new button state
  let curr = bean_keydown();

  // Accelerate upwards if pressed, downwards if not pressed
  // Cap velocity at 50 units/frame
  vel += curr ? 1 : -1;
  vel = max(-50, min(50, vel));

  // Apply velocity to position
  // Limit position to between 0 and 10000 units
  pos += vel;
  let prepos = pos;
  pos = max(0, min(10000, pos));

  // Zero velocity if position hits limit
  if (pos != prepos) {
    vel = 0;
  }

  // Show position
  bean_printi(pos);
}
