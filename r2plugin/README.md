# r2plugin

This directory contains the make and meson files required to link r2garlic
inside radare2 through the `libr/xps` external plugin flow.

```console
cd radare2
make -C libr/xps EXTERNAL_PLUGINS=r2garlic
make -C libr/core
```
