
# duster

<img src="media/bannerw.png" width="256">

a sleek strategy game for the gba

written with the D programming language. as far as i know, this is the only non-trivial gba game written in D!
D support comes from my work on [gba_dlang], providing library bindings, minimal runtime, and build scripts to run D on bare metal gba!

## build

### requirements

+ version control: [git](https://git-scm.com/)
+ gba toolchain: [devkitarm](https://devkitpro.org/wiki/Getting_Started) (packages: `tonc`, `grit`)
+ dlang llvm compiler: [ldc2](https://github.com/ldc-developers/ldc)
+ atlas packer: [crunch](https://github.com/redthing1/crunch)

### from source

go to `src/DusterGBA`
```sh
# fetch and update submodules
git submodule update --init --recursive
# build with debug flag
make build DEBUG=1
```

this will output `Duster.gba`

to check the `GAME_VERSION` of a built rom, run:

```sh
strings Duster.gba | grep '$DUSTER'
```

## preview

<img src="https://raw.githubusercontent.com/wiki/redthing1/duster/duster_test14.webp" width="512">

## license

- licensed under the [tuna fish license](LICENSE.md)
- copyright &copy; 2018-2022 bean machine. all rights reserved.
