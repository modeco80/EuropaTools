//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

import std.io;
import std.mem;

// Fundamental Types
struct Vec3 {
    float x;
    float y;
    float z;
    
    // std::print("Vec3: {:4.4f}, {:4.4f}, {:4.4f}",
    //    x,
    //    y,
    //    z 
    //);
};

struct Uvf {
    float u;
    float v;
};

struct Face {
   u16 faceX;
   u16 faceY;
   u16 faceZ; 
};

// Europa .MSH types

// a generic block
struct tMeshBlock<Item> {
    u16 count;
    
    // count == 0 means no items in block
    if(count != 0) {
        std::print("Block with {:6d} items", count);
        Item items[count];
    }
};

// Used for vertex colors.
struct VertexColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

struct UvBlockEntry {
    Uvf uvs[parent.parent.vertexBlock.count];
};

struct MeshPiece {
    char mtl_filename[while(std::mem::read_unsigned($-1, 1) != '\n')];    
    tMeshBlock<Vec3> vertexBlock;
    tMeshBlock<VertexColor> vertexColorBlock;
    tMeshBlock<Vec3> normalBlock;
    tMeshBlock<UvBlockEntry> uvBlock;
    tMeshBlock<Face> faceBlock;
};

struct MshFile {
    // This should just be read as lines in any real code.
    // I just do this to feel something
    char magic[while(std::mem::read_unsigned($-1, 1) != '\n')];
    char version[while(std::mem::read_unsigned($, 1) != '\n')];
    char dummy; // fuckkk dude
    
    // There is a meshpiece per material change I guess.
    tMeshBlock<MeshPiece> meshPieceBlock;
};

MshFile msh @ 0;