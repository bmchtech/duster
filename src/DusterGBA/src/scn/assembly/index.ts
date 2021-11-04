// The entry file of your WebAssembly module.

@external("duster", "bean_printi")
declare function bean_printi(x: i32): void;

@external("duster", "bean_keydown")
declare function bean_keydown(): i32;

var y = 0;

export function bean_start(): void {
  let x = bean_keydown();
  y = x;
}

export function bean_update(): void {
  y += 1;
  bean_printi(y);
}
