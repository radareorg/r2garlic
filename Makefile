GARLIC_SRC  = garlic/src

R2_CFLAGS   ?= $(shell r2 -H R2_CFLAGS)
R2_LDFLAGS  ?= $(shell r2 -H R2_LDFLAGS)
R2_PLUGINS  ?= $(shell r2 -H R2_USER_PLUGINS)
R2_LIBEXT   ?= $(shell r2 -H R2_LIBEXT)
R2_INCDIR   ?= $(shell r2 -H R2_INCDIR)

CC ?= gcc
CFLAGS  = -Wall -Wextra -O2 -fPIC -fvisibility=default -std=c11
CFLAGS += -D_GNU_SOURCE -DGARLIC_NO_MAIN
CFLAGS += $(R2_CFLAGS)
CFLAGS += -I$(GARLIC_SRC)
CFLAGS += -I$(GARLIC_SRC)/common
CFLAGS += -I$(GARLIC_SRC)/libs/memory
CFLAGS += -I$(GARLIC_SRC)/libs/hashmap
CFLAGS += -I$(GARLIC_SRC)/libs/list
CFLAGS += -I$(GARLIC_SRC)/libs/bitset
CFLAGS += -I$(GARLIC_SRC)/libs/queue
CFLAGS += -I$(GARLIC_SRC)/libs/str
CFLAGS += -I$(GARLIC_SRC)/libs/zip
CFLAGS += -I$(GARLIC_SRC)/libs/threadpool
CFLAGS += -I$(GARLIC_SRC)/libs/trie
CFLAGS += -I$(GARLIC_SRC)/parser/class
CFLAGS += -I$(GARLIC_SRC)/parser/dex
CFLAGS += -I$(GARLIC_SRC)/parser/pe
CFLAGS += -I$(GARLIC_SRC)/parser/reader
CFLAGS += -I$(GARLIC_SRC)/dalvik
CFLAGS += -I$(GARLIC_SRC)/decompiler
CFLAGS += -I$(GARLIC_SRC)/decompiler/transformer
CFLAGS += -I$(GARLIC_SRC)/jvm
CFLAGS += -I$(GARLIC_SRC)/jar
CFLAGS += -I$(GARLIC_SRC)/apk
CFLAGS += -Isrc

CFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter
CFLAGS += -Wno-unused-but-set-variable -Wno-implicit-function-declaration
CFLAGS += -Wno-incompatible-pointer-types -Wno-misleading-indentation -Wno-format

LDFLAGS = -shared -fPIC $(R2_LDFLAGS) -lpthread

ifeq ($(R2_LIBEXT),dylib)
LDFLAGS += -dynamiclib -undefined dynamic_lookup
endif

GARLIC_C_SRCS = $(filter-out $(GARLIC_SRC)/garlic.c, \
	$(wildcard $(GARLIC_SRC)/common/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/memory/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/hashmap/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/list/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/bitset/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/queue/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/str/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/zip/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/threadpool/*.c) \
	$(wildcard $(GARLIC_SRC)/libs/trie/*.c) \
	$(wildcard $(GARLIC_SRC)/parser/class/*.c) \
	$(wildcard $(GARLIC_SRC)/parser/dex/*.c) \
	$(wildcard $(GARLIC_SRC)/parser/pe/*.c) \
	$(wildcard $(GARLIC_SRC)/parser/reader/*.c) \
	$(wildcard $(GARLIC_SRC)/dalvik/*.c) \
	$(wildcard $(GARLIC_SRC)/decompiler/*.c) \
	$(wildcard $(GARLIC_SRC)/decompiler/transformer/*.c) \
	$(wildcard $(GARLIC_SRC)/jvm/*.c) \
	$(wildcard $(GARLIC_SRC)/jar/*.c) \
	$(wildcard $(GARLIC_SRC)/apk/*.c) \
)

PLUGIN_SRC = src/r2garlic.c src/memstream.c

GARLIC_OBJS = $(GARLIC_C_SRCS:.c=.o)
PLUGIN_OBJ  = $(PLUGIN_SRC:.c=.o)

PLUGIN = core_r2garlic.$(R2_LIBEXT)

.PHONY: all clean install user-install user-uninstall help

all: $(PLUGIN)

$(PLUGIN): $(PLUGIN_OBJ) libgarlic.a
	$(CC) $(LDFLAGS) -o $@ $(PLUGIN_OBJ) -L. -lgarlic

libgarlic.a: $(GARLIC_OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PLUGIN_OBJ): src/r2garlic.h src/memstream.h

ifeq ($(R2_LIBEXT),dll)
clean:
	del /Q $(GARLIC_OBJS) $(PLUGIN_OBJ) libgarlic.a $(PLUGIN) 2>nul || exit 0
else
clean:
	rm -f $(GARLIC_OBJS) $(PLUGIN_OBJ) libgarlic.a $(PLUGIN)
endif

ifeq ($(R2_LIBEXT),dll)
user-install: $(PLUGIN)
	if not exist "$(R2_PLUGINS)" mkdir "$(R2_PLUGINS)"
	copy $(PLUGIN) "$(R2_PLUGINS)\\"

user-uninstall:
	if exist "$(R2_PLUGINS)\\core_r2garlic.$(R2_LIBEXT)" del "$(R2_PLUGINS)\\core_r2garlic.$(R2_LIBEXT)"
else
user-install: $(PLUGIN)
	mkdir -p $(R2_PLUGINS)
	cp -f $(PLUGIN) $(R2_PLUGINS)/

user-uninstall:
	rm -f $(R2_PLUGINS)/core_r2garlic.$(R2_LIBEXT)
endif

help:
	@echo "r2garlic - Garlic DEX/Dalvik decompiler plugin for radare2"
	@echo ""
	@echo "Targets:"
	@echo "  all            Build the plugin (default)"
	@echo "  clean          Remove build artifacts"
	@echo "  user-install   Install plugin to radare2 user plugins directory"
	@echo "  user-uninstall Remove plugin from radare2 user plugins directory"
	@echo ""
	@echo "Variables:"
	@echo "  CC             C compiler (default: gcc)"
	@echo "  R2_CFLAGS      radare2 C flags (from r2 -H)"
	@echo "  R2_LDFLAGS     radare2 link flags (from r2 -H)"
	@echo "  R2_PLUGINS     radare2 plugin directory (from r2 -H)"
	@echo ""
	@echo "Usage:"
	@echo "  make"
	@echo "  make user-install"
	@echo "  r2 classes.dex -c 'pd:G'"
