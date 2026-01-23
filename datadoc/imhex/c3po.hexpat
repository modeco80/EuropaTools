//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

import std.mem;

struct zString { char string[]; };

struct Vec3 {
    float x;
    float y;
    float z;
};

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

/// script type. A bit of a TODO.
enum ScriptType : u8 {
};

/// The serialized version of an Europa CValue.
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
        (EValueType::Null): {}
        (EValueType::Error): {}
        //(_): {}
    }
};

/// Describes a single variable/property value.
struct CVariableInfo<auto version> {
    char variableName[];
    if(version > 2) {
        ScriptType scriptType;
        u8 extendedAttributes;
    } else {
        ScriptType scriptType;
    }
    CValue defaultValue;
};

// shared types (these never changed encoding between versions)
struct WeaponEntry {
    char weaponName[];
    char className[];
    char emitterName[];
};

struct WeaponGroup {
    char name[];
    u8 count;
    WeaponEntry entries[count];
};


// v5 types

// force power data. I assume it was encoded like this in
// ver4 too, but ver4 is useless to document.
struct ForcePowerKind {
    char kind[];
    char className[];
};

struct SimpleProperty {
    char name[];
    CValue value;
};

struct ForcePowerEntry {
    char name[];
    u8 countKinds;
    ForcePowerKind subKinds[countKinds];
    u8 propertyCount;
    SimpleProperty properties[propertyCount];
};

struct GroupEntry {
    char name[];
    char className[];
    char unkName[];

    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
    Vec3 v4;
};

struct GroupPropertyEntry {
    char objectName[];
    u8 propertyCount;
    SimpleProperty groupProperties[propertyCount];
};

struct CSFClassDetails<auto version> {
    char modelFileName[];
    // weapon groups
    u8 countWeaponGroups;
    WeaponGroup weaponGroups[countWeaponGroups];

    u8 countForcePowerEntries;
    ForcePowerEntry entries[countForcePowerEntries];

    u8 countGroup;
    GroupEntry groupEntities[countGroup];

    u8 countGroupProperties;
    GroupPropertyEntry groupProperties[countGroupProperties];
};

struct CSFWeaponDetails<auto version> {
    u8 unkPropCount;
    CVariableInfo<version> unkProperties[unkPropCount];
};

struct CSFForcePowerDetails<auto version> {
    char forcePowerClassName[];
    u8 unkPropCount;
    CVariableInfo<version> unkProperties[unkPropCount];
};

/// Class information.
struct CClassInfo<auto version> {
    if(version == 5) {
        char className[];
        char baseClassName[];
        u8 variableCount;
        CVariableInfo<version> variables[variableCount];

        // In version 5, CClassInfo gained a factory type ID.
        // This type ID matches what kind of class will be added to the GOD.
        u32 classFactoryTypeId;
        match(classFactoryTypeId) {
            (10000): { CSFClassDetails<version> details; }
            (10001): { CSFWeaponDetails<version> details; }
            (10002): { CSFForcePowerDetails<version> details; }
            (_): {}
        }
    } else {
        char className[];
        char baseClassName[];
        char modelFileName[];

        u8 stringCount;
        zString strings[stringCount];

        // This could be dependencies? Unsure
        u8 variableCount;
        CVariableInfo<version> variables[variableCount];

        u8 weaponGroupCount;
        WeaponGroup weaponGroup[weaponGroupCount];

        if(version == 4) {
            // This presumably is force power count.
            // However, all version 4 files we have are a part of
            // PC or Special Edition, which don't emit force power
            // information.
            u8 pad;
        }
    }
};

/// Weapon information.
struct CWeaponInfo<auto version> {
    char className[];
    char baseClassName[];
    u8 variableCount;
    CVariableInfo<version> variables[variableCount];
};

// used in versions less than 5
enum ClassType : u8 {
    Invalid = 0,
    Class = 1,
    Weapon = 2,
    // This was assigned for force powers in
    // version 4, but ultimately goes unused.
    // This was probably left as a accidental leftover
    // from Special Edition's engine actually being a early
    // beta of Jedi Starfighter.
    // (Its LECGame module is just SF hacked up to work, so it doesn't
    // have force powers, meaning the only thing remaining is GOD support)
    ForcePower = 3,
};

struct c3poFile {
    char magic[0x14+1];
    u8 revision;
    u16 version;

    if(version == 5) {
        // Version 5/JSF unifies everything into CClassInfo.
        CClassInfo<version> class;
    } else {
        // Versions 2..4 have mostly the same binary layout, excluding
        // version 4 adding extendedAttributes to CVariableInfo.
        //
        // Interestingly, version 4 (used on PC and Special Edition)
        // supports force powers from Jedi Starfighter
        // (this is most definitely beta code from JSF)
        ClassType type;
        match(type) {
            (ClassType::Class): {
                CClassInfo<version> class;
            }
            (ClassType::Weapon): {
                CWeaponInfo<version> weapon;
            }
            (_): {}
        }
    }
};

c3poFile file @ 0;
