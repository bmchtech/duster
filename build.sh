#!/usr/bin/env bash
#
pushd . && git submodule update --init --recursive && cd src/DusterGBA && make clean && make build && popd
