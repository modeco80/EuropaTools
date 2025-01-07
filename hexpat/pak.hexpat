//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#pragma endian little
// Big archives need a big pattern limit
#pragma pattern_limit 0x40000
#pragma array_limit 0x40000

namespace europa {

	struct PakHeader {
		char magic[16]; // "Europa Packfile\0"
		
		// Doesn't include magic
		u16 headerSize;

		// 0x3 - PMDL
		// 0x4 - Starfighter
		// 0x5 - Jedi Starfighter
		u16 Version;
		u8 pad;

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		// Unix epoch timestamp (libc time()) of when the archive was created
		u32 archiveCreationTime;

		// Set to 0 in basically every file
		u32 reserved;
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

		// Seems to be the same as he header.
		u32 creationTime;
	};

	struct PakFile {
		PakHeader header;
		PakTocEntry toc[header.fileCount] @ header.tocOffset;
	};

} // namespace europa

europa::PakFile pak @ 0x0;