
# docker

## docker build

```sh
git clone --recursive https://github.com/redthing1/duster duster_dockerbuild
pushd duster_dockerbuild
docker run -it --rm -v $(pwd):/source --user "$(id -u):$(id -g)" xdrie/dkarm_gba_docker:v0.3 -l -c "pushd . && git submodule update --init --recursive && cd src/DusterGBA && make clean && make build && popd"
popd
```
