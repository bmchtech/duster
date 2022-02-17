
# docker

## docker build

```sh
git clone --recursive https://github.com/redthing1/duster duster_dockerbuild
pushd duster_dockerbuild
docker run -it --rm -v $(pwd):/source dkarm_gba_docker:local -l -c "pushd . && git submodule update --init --recursive && cd src/DusterGBA && make clean && make build && chmod -R 777 . && popd"
popd
```
