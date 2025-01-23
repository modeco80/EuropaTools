# Europa Package Files (.PAK)

Package files are used by the Europa engine's core filesystem APIs to boost file performance.

They are not compressed or encrypted in any way.

## Versions

| Version | Estimated dates                                                           |
| ------- | ------------------------------------------------------------------------- |
| 3       | ? - July 2000. Seen in very early PMDL exports only                       |
| 4       | August 2000 - Feburary 2001 (Used for packages built closer to ship date) |
| 5       | Sometime 2001 - March 2002 (Used for Jedi Starfighter)                    |

Versions 3 and 4 are relatively similar, with only minor differences.

## Shared concepts

The package file layout is relatively simple. The header is at the start, the packaged file data is after, and the TOC is placed after all file data.

Essentially, visualized, it looks something like this:

```
--------- HEADER ---------
|||||||| file data |||||||
|||||||| file data |||||||
|||||||| file data |||||||
|||||||| file data |||||||
|||||||| file data |||||||
|||||||| file data |||||||
-------- TOC DATA --------
```

The header starts with a 16-character magic string, which in all versions is `Europa Packfile\0`.

Strings are Pascal-style length prefixed strings, written with a `u8` length prefix. The length prefix includes the C `\0` termination.

Every TOC entry starts with a Pascal string of the path name. For simplicity, structure descriptions only describe the static members of the TOC.

## Version 3

This version is the earliest version the game can parse. This was probably left in because a number of Starfighter .pmdls (renamed .pak's for models) were last exported around May - August 2000.

### Header

| Offset | Type       | Name         | Description                                  |
| ------ | ---------- | ------------ | -------------------------------------------- |
| `0x0`  | `char[16]` | magic        | The magic.                                   |
| `0x10` | `u8`       | revision     | Unknown value. Always `0x1A`.                |
| `0x11` | `u8`       | [padding]    | Padding field (maybe `revision` is u16?).    |
| `0x12` | `u16`      | version      | Version.                                     |
| `0x14` | `u32`      | tocOffset    | Offset to Table Of Contents (TOC)            |
| `0x18` | `u32`      | tocSize      | Size of TOC in bytes.                        |
| `0x1c` | `u32`      | fileCount    | Amount of files stored in this package file. |
| `0x20` | `u32`      | creationTime | Unix timestamp of when archive was created.  |
| `0x24` | `u32`      | reserved     | Set to 0.                                    |

### TOC

| Offset | Type  | Name         | Description                                             |
| ------ | ----- | ------------ | ------------------------------------------------------- |
| `0x0`  | `u32` | offset       | Offset in package file where the data for this file is. |
| `0x4`  | `u32` | size         | The size of the file.                                   |
| `0x8`  | `u16` | unknown      | Purpose is unknown, it is always `0xffff` (s16 -1).     |
| `0xA`  | `u32` | creationTime | Unix timestamp of file modification time.               |

## Version 4

This version was used for most of Starfighter's packages that were packaged closer to the final release.

### Header

| Offset | Type       | Name         | Description                                  |
| ------ | ---------- | ------------ | -------------------------------------------- |
| `0x0`  | `char[16]` | magic        | The magic.                                   |
| `0x10` | `u8`       | revision     | Unknown value. Always `0x1A`.                |
| `0x11` | `u8`       | [padding]    | Padding field (maybe `revision` is u16?).    |
| `0x12` | `u16`      | version      | Version.                                     |
| `0x14` | `u8`       | [padding]    | Another padding field.                       |
| `0x16` | `u32`      | tocOffset    | Offset to Table Of Contents (TOC)            |
| `0x1a` | `u32`      | tocSize      | Size of TOC in bytes.                        |
| `0x1e` | `u32`      | fileCount    | Amount of files stored in this package file. |
| `0x22` | `u32`      | creationTime | Unix timestamp of when archive was created.  |
| `0x26` | `u32`      | reserved     | Set to 0.                                    |

### TOC

| Offset | Type  | Name         | Description                                             |
| ------ | ----- | ------------ | ------------------------------------------------------- |
| `0x0`  | `u32` | offset       | Offset in package file where the data for this file is. |
| `0x4`  | `u32` | size         | The size of the file.                                   |
| `0x8`  | `u32` | creationTime | Unix timestamp of file modification time.               |

## Version 5

This version was used for Jedi Starfighter, and gains some features, including:

- The ability for file data to be aligned to ISO-9660 sector boundaries

The header was expanded a bit.

### Header

| Offset | Type       | Name          | Description                                      |
| ------ | ---------- | ------------- | ------------------------------------------------ |
| `0x0`  | `char[16]` | magic         | The magic.                                       |
| `0x10` | `u8`       | revision      | Unknown value. Always `0x1A`.                    |
| `0x11` | `u8`       | [padding]     | Padding field (maybe `revision` is u16?).        |
| `0x12` | `u16`      | version       | Version.                                         |
| `0x14` | `u8`       | [padding]     | Another padding field.                           |
| `0x15` | `u32`      | tocOffset     | Offset to Table Of Contents (TOC)                |
| `0x19` | `u32`      | tocSize       | Size of TOC in bytes.                            |
| `0x1d` | `u32`      | fileCount     | Amount of files stored in this package file.     |
| `0x21` | `u32`      | creationTime  | Unix timestamp of when archive was created.      |
| `0x25` | `u32`      | reserved      | Set to 0.                                        |
| `0x29` | `u32`      | sectorSize    | Must be set if `sectorAligned` is set.           |
| `0x2c` | `u8`       | sectorAligned | Set to 1 if this package file is sector aligned. |
| `0x2e` | `u8`       | [padding]     | Padding.                                         |

### TOC (Sector Aligned)

| Offset | Type  | Name         | Description                                             |
| ------ | ----- | ------------ | ------------------------------------------------------- |
| `0x0`  | `u32` | offset       | Offset in package file where the data for this file is. |
| `0x4`  | `u32` | size         | The size of the file.                                   |
| `0x8`  | `u32` | startLBA     | The start LBA of the file.                              |
| `0x8`  | `u32` | creationTime | Unix timestamp of file modification time.               |

### TOC (Not Sector Aligned)

Same as version 4.
