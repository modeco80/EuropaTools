//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

import std.mem;

// TODO: Many files actually use this, so it might be a good idea
// to split this into another include.
enum EValueType : u8 {
    Null = 0x0,
    String = 0x1,

    Bool = 0x3,

    Char = 0x4,
    UChar = 0x5,

    Short = 0x6,
    UShort = 0x7,

    Long = 0x8,
    ULong = 0x9,

    Float = 0xa,
    // the game has 0xb, but this can't ever
    // be read from a bin, so it's not useful.

    Double = 0xc, //? Just 8 bytes read.

    Error = 0xf,
};

struct CValue {
    EValueType type;
    match(type) {
        (EValueType::String): { char value[]; }
        (EValueType::Bool): { bool value; }
        (EValueType::Char): { s8 value; }
        (EValueType::UChar): { u8 value; }
        (EValueType::Short): { s16 value; }
        (EValueType::UShort): { u16 value; }
        (EValueType::Long): { s32 value; }
        (EValueType::ULong): { u32 value; }
        (EValueType::Float): { float value; }
        (EValueType::Double): { double value; }
        (_): {}
    }
};

struct CVariableInfo {
    char variableName[];
    u8 unk;
    CValue value;
};

enum ClassType : u8 {
    Invalid = 0,
    Class = 1,
    Weapon = 2,
    ForcePower = 3,
};

struct c3poFile {
    char magic[0x14+1];
    u8 revision;
    u16 version;
    ClassType type;

    match(type) {
        (ClassType::Class): {
            char className[];
            char baseClassName[];
            char modelFileName[];
            // it seems classes have an extra byte of padding which breaks
            // reading until EOF.
            //CVariableInfo variables[11];
        }
        (ClassType::Weapon): {
            char className[];
            char baseClassName[];
            u8 unk;
            CVariableInfo variables[while(!std::mem::eof())];
        }
        (_): {}
    }

};

c3poFile file @ 0;
