
# duster

<img src="media/bannerw.png" width="256">

a sleek strategy game for the gba

## build

### deps

+ [git](https://git-scm.com/)
+ [devkitarm](https://devkitpro.org/wiki/Getting_Started) (packages: `tonc`, `grit`)
+ [nelua](https://github.com/edubart/nelua-lang/)
+ [crunch](https://github.com/redthing1/crunch)

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
