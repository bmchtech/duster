.PHONY: clean atlas

TOOLS_DIR ?= ../../../tools

IMGS_DIR := ../img
MAPS_DIR := ../map
RAWBGS_DIR := ../rawbg
GAMEMAPS_DIR := ../gamemap
SOUND_DIR := ../snd

ATLS_SRC := $(shell find $(IMGS_DIR) -mindepth 1 -type d)
ATLS_OUT := $(patsubst %,a_%_,$(notdir $(ATLS_SRC)))
ATLS_PNG := $(ATLS_OUT:%_=%_0.png)

IMGS_BIN := $(notdir $(ATLS_PNG:%.png=%.img.bin))

RAWBGS_SRC := $(shell find $(RAWBGS_DIR) -name '*.png')
RAWBGS_BIN := $(notdir $(RAWBGS_SRC:%.png=%.img.bin))

# MAPS_SRC := $(shell find $(MAPS_DIR) -name '*.tmx')
# MAPS_BIN := $(notdir $(MAPS_SRC:%.tmx=%.bin))

GAMEMAPS_SRC := $(shell find $(GAMEMAPS_DIR) -name '*.json')
GAMEMAPS_BIN := $(notdir $(GAMEMAPS_SRC:%.json=%.gmp.bin))

SOUND_SRC := $(shell find $(SOUND_DIR) -type f)
SOUND_BIN := soundbank.bin
SOUND_H := soundbank.h

GRIT_ATL_BPP := 4
GRIT_ATL_FLAGS := -gt -p -g -gB $(GRIT_ATL_BPP) -gu32 -pu32 -ftb -fh!
GRIT_RMP_BPP := 8
GRIT_RMP_FLAGS := -gt -p -m -g -gB $(GRIT_RMP_BPP) -gu32 -pu32 -ftb -fh!
CRUNCH_FLAGS := -p -j -b -s256 -p0

all: $(RAWBGS_BIN) $(IMGS_BIN) $(SOUND_BIN) $(SOUND_H) $(GAMEMAPS_BIN)

clean:
	rm -rfv *.bin
	rm -rfv *.b
	rm -rfv *.h
	rm -rfv atl*
	rm -rfv *.png
	rm -rfv *.hash
	rm -rfv *.json

# define a function: atlpng2grit(size), this will set Mw and Mh to size
atlpng2grit = grit "$<" $(GRIT_ATL_FLAGS) -Mh $(1) -Mw $(1) -o "$@"
rawmap2grit = grit "$<" $(GRIT_RMP_FLAGS) -Mh $(1) -Mw $(1) -o "$@"

# default rule for atlpng -> img bin
%.img.bin: %.png
	grit "$<" $(GRIT_ATL_FLAGS) -o "$@"

# import custom recipes
include custom.mk

# rule for building atlas pngs from sprite dirs
# build a_thing_
a_%_: ../img/%
	$(TOOLS_DIR)/bin/crunch_gen "$@" "$<" $(CRUNCH_FLAGS)
# convert a_thing_ to a_thing_0.png (fake recipe)
$(ATLS_PNG): $(ATLS_OUT)
	@true

# rule for building raw maps
%.img.bin: ../rawbg/%.png
	$(call rawmap2grit,1,0,0)

# rule for building soundbank
$(SOUND_BIN) $(SOUND_H): $(SOUND_SRC)
	mmutil -v $^ -osoundbank.bin -hsoundbank.h

# rule for building game maps
%.gmp.bin: $(GAMEMAPS_DIR)/%.json
	$(TOOLS_DIR)/bin/duster_mapc $< $@

# debug:
#     $(foreach var,$(.VARIABLES),$(info $(var) = $($(var))))