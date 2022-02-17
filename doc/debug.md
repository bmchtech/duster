# how to debug

use provided vscode `launch.json` to attach to mgba debugger.
to run mgba with open gdb:
```sh
make build DEBUG=1 && mgba-qt -g Duster.gba 
```

to disable `-O2`, pass `DEBUG=2`
