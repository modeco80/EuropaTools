//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

// YATF pattern

#include <std/io.pat>
#pragma endian little

namespace europa {

	// YATF header.
	struct TexHeader {
		char magic[4]; // 'YATF'
		
		// Flag descriptions:
		//
		// 0x1       - unknown? (always pressent)
		// 0x30000   - direct color (no palette)
		// 0x1000000 - uses alpha
		u32 flags;
		
		// Always zeroed.
		u32 zero;
		
		// these are swapped backwards
		// for some reason.
		u32 height;
		u32 width;
	};
	
	// A YATF file.
	struct TexFile {
		TexHeader header;
		
		if(header.flags & 0x1000000) {
			std::print("[YATF] this YATF uses alpha?");
		}
		
		if(!(header.flags & 0x30000)) {
			std::print("[YATF] this YATF is palettized");
			
			u32 palette[256];
			u8 bitmap[header.width * header.height];
		} else {
			std::print("[YATF] this YATF is direct color");
			
			u32 bitmap[header.width * header.height];
		}
	};

} // namespace europa


europa::TexFile tex @ 0x0;
