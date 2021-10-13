
# duster

a sleek strategy game for the gba

<img src="media/test6_retro.webp" width="512">

## build

### deps

+ git
+ devkitarm (packages: `tonc`, `grit`)
+ [crunch](https://github.com/redthing1/crunch)

### from source

go to `src/DusterGBA`
```sh
git submodule update --init --recursive
make build DEBUG=1
```

this will output `Duster.gba`