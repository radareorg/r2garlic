R2GARLIC_CORE_WD=$(LIBR)/xps/p/r2garlic
R2GARLIC_SRC=$(R2GARLIC_CORE_WD)/garlic/src

CFLAGS+=-D_GNU_SOURCE -DGARLIC_NO_MAIN
CFLAGS+=-I$(R2GARLIC_CORE_WD)/src
CFLAGS+=-I$(R2GARLIC_SRC)
CFLAGS+=-I$(R2GARLIC_SRC)/common
CFLAGS+=-I$(R2GARLIC_SRC)/libs/memory
CFLAGS+=-I$(R2GARLIC_SRC)/libs/hashmap
CFLAGS+=-I$(R2GARLIC_SRC)/libs/list
CFLAGS+=-I$(R2GARLIC_SRC)/libs/bitset
CFLAGS+=-I$(R2GARLIC_SRC)/libs/queue
CFLAGS+=-I$(R2GARLIC_SRC)/libs/str
CFLAGS+=-I$(R2GARLIC_SRC)/libs/zip
CFLAGS+=-I$(R2GARLIC_SRC)/libs/threadpool
CFLAGS+=-I$(R2GARLIC_SRC)/libs/trie
CFLAGS+=-I$(R2GARLIC_SRC)/parser/class
CFLAGS+=-I$(R2GARLIC_SRC)/parser/dex
CFLAGS+=-I$(R2GARLIC_SRC)/parser/pe
CFLAGS+=-I$(R2GARLIC_SRC)/parser/reader
CFLAGS+=-I$(R2GARLIC_SRC)/dalvik
CFLAGS+=-I$(R2GARLIC_SRC)/decompiler
CFLAGS+=-I$(R2GARLIC_SRC)/decompiler/transformer
CFLAGS+=-I$(R2GARLIC_SRC)/jvm
CFLAGS+=-I$(R2GARLIC_SRC)/jar
CFLAGS+=-I$(R2GARLIC_SRC)/apk
CFLAGS+=-Wno-unused-variable -Wno-unused-function -Wno-unused-parameter
CFLAGS+=-Wno-unused-but-set-variable -Wno-implicit-function-declaration
CFLAGS+=-Wno-incompatible-pointer-types -Wno-misleading-indentation
CFLAGS+=-Wno-format -Wno-sign-compare -Wno-cast-function-type-mismatch

R2GARLIC_C_SRCS=$(filter-out $(R2GARLIC_SRC)/garlic.c, \
	$(wildcard $(R2GARLIC_SRC)/common/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/memory/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/hashmap/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/list/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/bitset/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/queue/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/str/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/zip/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/threadpool/*.c) \
	$(wildcard $(R2GARLIC_SRC)/libs/trie/*.c) \
	$(wildcard $(R2GARLIC_SRC)/parser/class/*.c) \
	$(wildcard $(R2GARLIC_SRC)/parser/dex/*.c) \
	$(wildcard $(R2GARLIC_SRC)/parser/pe/*.c) \
	$(wildcard $(R2GARLIC_SRC)/parser/reader/*.c) \
	$(wildcard $(R2GARLIC_SRC)/dalvik/*.c) \
	$(wildcard $(R2GARLIC_SRC)/decompiler/*.c) \
	$(wildcard $(R2GARLIC_SRC)/decompiler/transformer/*.c) \
	$(wildcard $(R2GARLIC_SRC)/jvm/*.c) \
	$(wildcard $(R2GARLIC_SRC)/jar/*.c) \
	$(wildcard $(R2GARLIC_SRC)/apk/*.c) \
)

R2GARLIC_CORE_OBJ=$(R2GARLIC_C_SRCS:.c=.o) \
	$(R2GARLIC_CORE_WD)/src/r2garlic.o \
	$(R2GARLIC_CORE_WD)/src/memstream.o

EXTERNAL_STATIC_OBJS+=$(R2GARLIC_CORE_OBJ)
