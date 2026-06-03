# r2garlic

radare2 plugin for the [Garlic](https://github.com/neocanable/garlic) DEX/Dalvik decompiler.

## Build

```bash
git clone https://github.com/neocanable/r2garlic.git
cd r2garlic
git submodule init
git submodule update
make
```

### Meson

```bash
meson setup build-meson
meson compile -C build-meson
```

## Install

### From r2pm (recommended)

```bash
r2pm -Uci r2garlic
```

### From source

```bash
make user-install
```

With Meson:

```bash
meson install -C build-meson
```

Pass `-Dr2_plugindir=/path/to/plugins` to `meson setup` to override the radare2 plugin install directory.

### radare2 XPS

The `r2plugin/` directory supports linking r2garlic inside radare2 via `libr/xps`:

```bash
cd radare2
make -C libr/xps EXTERNAL_PLUGINS=r2garlic
make -C libr/core
```

## Uninstall

```bash
make user-uninstall
```

## Usage

### Commands

| Command | Description                              |
| ------- | ---------------------------------------- |
| `pd:G`  | Decompile the method at current seek     |
| `pd:Gc` | Decompile the full class at current seek |
| `pd:Ga` | Decompile all classes                    |
| `pd:Gi` | Show DEX file info (dexdump)             |
| `pd:Gs` | Output smali for current class           |
| `pd:G?` | Show help                                |
