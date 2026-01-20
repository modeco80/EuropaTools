//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma endian little
// Big archives need a big pattern limit
#pragma pattern_limit 0x40000
#pragma array_limit 0x40000

namespace europa {

	struct PakHeader {
		char magic[16]; // "Europa Packfile\0"
		
		// Doesn't include magic
		u8 revision;

		// This seems to be the start of the actual header
		u8 pad;

		// 0x3 - PMDL
		// 0x4 - Starfighter
		// 0x5 - Jedi Starfighter
		u16 Version;

		// Not sure what they were planning to use this extra byte for.
		// It's always 0x0 and only present in version 4 onwards.
		if(Version > 0x3) {
			u8 pad2;
		}

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		// Unix epoch timestamp (libc time()) of when the archive was created
		u32 archiveCreationTime;

		// Set to 0 in basically every file
		u32 reserved;

		// Version 5 has additional fields to support sector-alignment
		if(Version == 5) {
			u32 sectorSize;
			u8 sectorAligned;
			u8 pad3;
		}
	};

	// "Pascal" string used
	// in the TOC.
	struct PakTocPString {
		u8 len;

		if(len != 0)
			char string[len];
	};

	// A Toc entry
	struct PakTocEntry {
		PakTocPString fileName;

		u32 offset;
		u32 size;
        
		if(parent.header.Version == 3) {
			u16 unk; // always 0xff. idk what this is for
		}

		if(parent.header.Version == 5) {
			if(parent.header.sectorAligned == 0x1) {
				// Start LBA of the file (relative to the start LBA of the package file itself)
				u32 startLBA;
			}
		}

		// Seems to be the same as he header.
		u32 creationTime;
	};

	struct PakFile {
		PakHeader header;
		PakTocEntry toc[header.fileCount] @ header.tocOffset;
	};

} // namespace europa

europa::PakFile pak @ 0x0;