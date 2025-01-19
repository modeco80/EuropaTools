## EuropaTools

Tools for working with LEC Europa based games (Star Wars: Starfighter & Star Wars: Jedi Starfighter), written in C++20. For nyoom. And also because why not.

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

Provides IO readers and writers for Europa data files.

#### File Format Support

> [!NOTE]  
> Things marked as unsupported will probably end up being supported at some point; so don't treat this as "final".


| Type                  | Support                         |
| --------------------- | ------------------------------- |
| Package (.pak, .pmdl) | Read/Write (all versions)       |
| PS2 Texture (.tex)    | Read-only export (all versions) |
| Mesh (.msh)           | Currently unsupported (planned) |
| Model (.mdl)          | Currently unsupported (planned) |
| Animation (.ani)      | Currently unsupported (planned) |
| GOD Data (.c3po)      | Currently unsupported           |
| Mission (.wrl)        | Currently unsupported           |
| Scene (.scene)        | Currently unsupported           |

## The Tools

### `Adam`

Planned to ultimately be an open-source equlivant to the `Eve` mission authoring tool used in development of Europa based games. With probably at least some UI design nicities that have been learned in the past 2 decades.

Feature set:

- [] working opengl imgui sex
- ... ?

### `eupak`

Swiss army knife for Europa packfiles.

Can create, extract, and show info on them.

### `eutex`

A utlity for dumping Europa PS2 .tex files. (creation later)
