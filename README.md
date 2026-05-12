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

## Install

```bash
make user-install
```

## Uninstall

```bash
make user-uninstall
```

## Usage

### Commands

| Command | Description                         |
| ------- | ----------------------------------- |
| `pd:G`  | Decompile the class at current seek |
| `pd:Ga` | Decompile all classes               |
| `pd:Gi` | Show DEX file info (dexdump)        |
| `pd:Gs` | Output smali for current class      |
| `pd:G?` | Show help                           |
