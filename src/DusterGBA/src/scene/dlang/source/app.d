extern(C): // disable D mangling

void my_printf(char* foo);

// add two ints
int add(int a, int b) {
  return a + b;
}

void say_hello() {
    my_printf(cast(char *)"hello world!");
}

// seems to be the required entry point
void _start() {
}

