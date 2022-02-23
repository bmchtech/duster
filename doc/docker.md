
# docker/podman container building

1. clone repository
2. enter directory of cloned repository
3. based on container program, follow below instructions
## docker

```sh
docker run -it --rm -v $(pwd):/source --user "$(id -u):$(id -g)" xdrie/dkarm_dusk:v0.5 -l -c "pushd . && git submodule update --init --recursive && cd src/DusterGBA && make clean && make build && popd"
```

## podman (rootless)

```sh
podman unshare chown "$(id -u):$(id -g)" -R .
podman run -it --rm -v $(pwd):/source --user "$(id -u):$(id -g)" docker.io/xdrie/dkarm_dusk:v0.5 -l -c "pushd . && git submodule update --init --recursive && cd src/DusterGBA && make clean && make build && popd"
```
