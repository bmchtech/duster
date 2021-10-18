
# duster

<img src="media/bannerw.png" width="256">

a sleek strategy game for the gba

## build

### deps

+ git
+ devkitarm (packages: `tonc`, `grit`)
+ compilers and build tools: `make`, `cmake`, C compiler, D compiler

### from source

go to `src/DusterGBA`
```sh
git submodule update --init --recursive
make build DEBUG=1
```

this will output `Duster.gba`

to check the `GAME_VERSION` of a built rom, run: `strings Duster.gba | grep '$DUSTER'`.

## preview

<img src="https://raw.githubusercontent.com/wiki/redthing1/duster/duster_test14.webp" width="512">
