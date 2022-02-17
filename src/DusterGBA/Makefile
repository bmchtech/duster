PATH := $(DEVKITARM)/bin:$(PATH)

TITLE		:= Duster

#  Project settings

NAME		:= $(TITLE)
SOURCE_DIR 	:= src
LIB_DIR    	:= lib
TOOLS_DIR	:= ../tools
LIBTONC    := ../../lib/libtonc
DUSK_DIR   := ../../lib/dusk/src/dusk
DATA_DIR   := asset/build
SPECS      := -specs=gba.specs

# Compilation settings

CROSS	?= arm-none-eabi-
AS	:= $(CROSS)as
CC	:= $(CROSS)gcc
LD	:= $(CROSS)gcc
OBJCOPY	:= $(CROSS)objcopy
LDC     := ldc2
NELUA	:= nelua

ARCH	:= -mthumb-interwork -mthumb

INCFLAGS := -I$(DUSK_DIR)/include -I$(LIBTONC)/include -I$(DEVKITPRO)/libgba/include -I$(SOURCE_DIR) -I$(DATA_DIR)
LIBFLAGS := -L$(DUSK_DIR)/lib -ldusk -L$(LIBTONC)/lib -ltonc -L$(DEVKITPRO)/libgba/lib -lmm
ASFLAGS	:= -mthumb-interwork
CFLAGS	:= $(ARCH) -Wall -Werror -Wno-error=unused-variable -fno-strict-aliasing -mcpu=arm7tdmi -mtune=arm7tdmi $(INCFLAGS) $(LIBFLAGS)
LDFLAGS	:= $(ARCH) $(SPECS) $(LIBFLAGS) -Wl,-Map,$(TITLE).map
DFLAGS  := -betterC --defaultlib=no -mtriple=arm-none-eabi -mcpu=arm7tdmi \
				-mattr=+strict-align,+loop-align\
				-I$(SOURCE_DIR) \
				--d-version=DevkitARM --d-version=CRuntime_Newlib_GBA --d-version=_GBA \
				-function-sections
NELUA_FLAGS := -P noentrypoint -P nogc -P nochecks --release --add-path $(SOURCE_DIR)/lua

ASSET_MKFLAGS := TOOLS_DIR=$(addprefix $(CURDIR)/,$(TOOLS_DIR))
DUSK_MKFLAGS :=
TONC_MKFLAGS := -j$(nproc)

DSTDLIB := ../../lib/gba_dlang/libd
DINCFLAGS :=
DINCDIRS := $(DSTDLIB) $(DUSK_DIR)/d_src
# add an -I include for each directory in the list
DFLAGS += $(foreach dir,$(DINCDIRS),-I$(dir))

ifeq ($(DEBUG),1)
	CFLAGS += -O2 -g -DDEBUG
	DFLAGS += -O2 -g --d-version=DEBUG
	DUSK_MKFLAGS += DEBUG=1
else ifeq ($(DEBUG),2)
	CFLAGS += -g -DDEBUG
	DFLAGS += -g --d-version=DEBUG --d-version=DEBUG2
	DUSK_MKFLAGS += DEBUG=1
else
	# non-debug
	CFLAGS += -O2 -fomit-frame-pointer -ffast-math
	DFLAGS += -O2 -frame-pointer=none -ffast-math
endif

# get revision id from git
REVISIONID := $(shell printf "r%s.%s" "$(shell git rev-list --count HEAD)" "$(shell git rev-parse --short HEAD)")
# append debug flag if in debug mode
ifneq ($(DEBUG),0)
	REVISIONID := $(addsuffix .d$(DEBUG),$(REVISIONID))
endif
$(info build id: $(REVISIONID))

CFLAGS += -DGAME_BUILD_ID="\"$(REVISIONID)\""

.PHONY : build clean

# Find and predetermine all relevant source files

APP_MAIN_SOURCE := $(shell find $(SOURCE_DIR) -name '*main.c')
APP_MAIN_OBJECT := $(APP_MAIN_SOURCE:%.c=%.o)
APP_SOURCES_C     := $(shell find $(SOURCE_DIR) -name '*.c' ! -name "*main.c"  ! -name "*.test.c")
APP_SOURCES_S     := $(shell find $(SOURCE_DIR) -name '*.s')
APP_OBJECTS_C     := $(APP_SOURCES_C:%.c=%.o)
APP_OBJECTS_S     := $(APP_SOURCES_S:%.s=%.o)
APP_SOURCES_D     := $(shell find $(DINCDIRS) $(SOURCE_DIR) -name '*.d')
APP_OBJECTS_D     := $(APP_SOURCES_D:%.d=%.o)
APP_SOURCES_NELUA := $(shell find $(SOURCE_DIR) -name '*.lua')
APP_NELUA_SCRIPTS := $(shell find $(SOURCE_DIR) -name '*.nelua')
APP_OBJECTS_NELUA := $(APP_SOURCES_NELUA:%.lua=%.o)
APP_OBJECTS		  := $(APP_OBJECTS_C) $(APP_OBJECTS_S) $(APP_OBJECTS_D) $(APP_OBJECTS_NELUA)

# Build commands and dependencies

.PHONY: libs assets tools

build: assets libs $(NAME).gba

no-content: libs $(NAME)-code.gba

libs:
	cd $(DUSK_DIR) && make $(DUSK_MKFLAGS)
	cd $(LIBTONC) && make $(TONC_MKFLAGS)

tools: $(TOOLS_DIR)/bin/crunch_gen $(TOOLS_DIR)/bin/duster_mapc

assets: tools
	cd asset/build && make $(ASSET_MKFLAGS)

# Tools build
$(TOOLS_DIR)/bin/crunch_gen:
	cd $(TOOLS_DIR)/crunch && mkdir -p build && cd build && cmake .. && make && ln -rsf crunch_gen ../../bin
$(TOOLS_DIR)/bin/duster_mapc:
	cd $(TOOLS_DIR)/duster_mapc && dub build && ln -rsf duster_mapc ../bin

# GBA ROM Build

$(NAME).gba : $(NAME)-code.gba $(NAME).gbfs
	cat $^ > $(NAME).gba

$(NAME)-code.gba : $(NAME).elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@ -t$(NAME)
	padbin 256 $@

$(NAME).elf : $(APP_OBJECTS) $(APP_MAIN_OBJECT)
	$(LD) $^ $(LDFLAGS) -o $@

$(APP_OBJECTS_C) : %.o : %.c assets libs
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_OBJECTS_S) : %.o : %.s
	$(CC) $(CFLAGS) -c $< -o $@

$(APP_OBJECTS_D) : %.o : %.d
	$(LDC) $(DFLAGS) -c -of=$@ $<

$(APP_OBJECTS_NELUA) : %.o : %.lua $(APP_NELUA_SCRIPTS)
	$(NELUA) -B $(NELUA_FLAGS) --cc $(CC) --cflags="$(CFLAGS)" -o $@ $<

$(APP_MAIN_OBJECT) : $(APP_MAIN_SOURCE)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME).gbfs: assets
	gbfs $@ $(shell find $(DATA_DIR) -name '*.bin')

clean:
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.map
	@rm -fv *.sav
	@rm -fv *.gbfs
	@rm -rf $(APP_OBJECTS)
	@rm -rf $(APP_MAIN_OBJECT)
	cd $(DUSK_DIR) && make clean
	cd asset/build && make clean
