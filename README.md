## EuropaTools

Tools for working with LEC Europa based games (Star Wars: Starfighter & Star Wars: Jedi Starfighter).

As per usual for lily, written in C++20.


## Building

```bash
$ git clone https://github.com/modeco80/EuropaTools.git
$ cd EuropaTools
$ cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
$ cmake --build build -j $(nproc)
# ... profit?
```

## The Libraries

### `libeuropa`

Provides IO readers and writers for data files, along with the structures.

Structure documentation is seperately managed as a .hexpat in [/hexpat](https://github.com/modeco80/EuropaTools/tree/master/hexpat).

## The Tools

## `europa_pack_extractor`

Staging tool to extract paks. Will be removed when eupak is ready.

### `pakcreate`

Staging tool to create paks.

### `paktest`

A test tool to test building paks, used during development.

### `texdump`

Dumper for PS2 `YATF` texture files. Mostly working, but slight WIP.

### `eupak`

Swiss army knife for Europa packfiles.

Can create, extract, and show info on them.

