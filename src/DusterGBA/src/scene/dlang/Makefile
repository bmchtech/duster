SOURCE_DIR := .
APP_SOURCES := $(shell find $(SOURCE_DIR) -name '*.d')
APP_COMPILED_WASM := $(shell find $(SOURCE_DIR) -name '*.wasm')

.PHONY: all
all: pack

.PHONY: dbuild
dbuild: $(APP_SOURCES)
	dub build -b release --compiler=ldc2 --arch=wasm32-unknown-unknown-wasm

.PHONY: pack
pack: dbuild $(APP_COMPILED_WASM)
	python3 ../packer.py dlang_test_wasm.wasm bean_dlang.wasm.h
