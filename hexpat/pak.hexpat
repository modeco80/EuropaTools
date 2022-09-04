
#pragma endian little

namespace europa {

	struct PakHeader {
		char magic[16]; // "Europa Packfile\0"
		
		// Doesn't include magic
		u16 headerSize;

		// 0x4 - Starfighter
		// 0x5 - other game
		u16 Version;
		u8 pad;

		u32 tocOffset;

		// Dunno what this is
		u32 unk;

		u32 fileCount;

		// Labeled as this in the quickbms script
		u32 archiveCreationTime;

		u32 reserved;//?
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
		//PakTocEntry toc[header.fileCount] @ header.tocOffset;
	};

} // namespace europa

europa::PakFile pak @ 0x0;