# Europa Model Mesh (.msh)

> [!WARNING]  
> This document is mostly complete, however some pieces of it are not.

Europa mesh files are used to store submodel data. They are referenced by .mdl files.

# Magic/version

Magic/version is indicated as 2 Windows (CR-LF) formatted lines:

- `MESH\r\n`
- `[version]\r\n`

A file which does not start with the first line SHALL be rejected as a invalid .msh regardless of version.

The following versions are known to exist:

| Version | Description                                                                                                               |
| ------- | ------------------------------------------------------------------------------------------------------------------------- |
| 1.2     | Very early text format. Used by a grand total of one accidentally shipped(?) mesh (RESOURCE.PAK `model\default.msh`) file |
| 2.0     | Binary format. Used by literally everything else                                                                          |

# Version 1.2

> [!CAUTION]
> This format is currently not supported by libeuropa, since it can more than likely be imported by hand. Maybe for historic curiousity I'll document it later, but don't expect much.

# Version 2.0

### Block

Mesh 2.0 files use a block structure for lists of information.

A block consists of a 16-bit unsigned integer of count of items (not bytes!) in that block, then the serialized items themselves.

A empty block will always be serialized as just `00 00` regardless of type.

### Mesh piece

A mesh piece is used to change material or potentionally to split a mesh if it exceeds 64k verts.

In practice it's usually probably the first one.

First the material name is read (until a new line character is read). This corresponds to a <!-- FIXME: Write doc --> .mtl material file the game will also parse.

Next, the following blocks are read and parsed:

- Vertex block. This block corresponds of Vec3s (your usual float x, float y, float z deal) which repressent each vertex.
- Vertex color block. Consists of a struct of rgba colors for each vertex.
- Normal block. Consists of Vec3s which define normal. May possibly not be pressent (normals probably have to be generated then.)
- UV block. This consists of a set of UV coordinates over all verticies (and there can be multiple UVs, but I haven't fully worked this out..)
- Face block. This consists of 3 uint16 face indicies for XYZ for each vertex.

In version 2.0, the mesh file consists of a block of mesh pieces, except there will be at least one in a valid mesh file.
