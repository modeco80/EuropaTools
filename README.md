## EuropaTools

Tools for working with LEC Europa based games (Star Wars: Starfighter & Star Wars: Jedi Starfighter).

As per usual for lily, written in C++20.


## Building

```bash
$ git clone https://git.computernewb.com/modeco80/EuropaTools.git
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

### `Adam`

Planned to be an open-source equlivant to the `Eve` mission authoring tool used in development of Europa based games.

Feature set:

- [] working opengl imgui sex
- ... ?

### `texdump`

Dumper for PS2 `YATF` texture files. Mostly working, but slight WIP.

### `eupak`

Swiss army knife for Europa packfiles.

Can create, extract, and show info on them.

