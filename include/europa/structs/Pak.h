//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_STRUCTS_PAK_H
#define EUROPA_STRUCTS_PAK_H

#include <cstdint>

#include <europa/structs/ImHexAdapter.h>

namespace europa::structs {

	enum class PakVersion : u16 {
		Starfighter = 0x4,
		Ver2 = 0x5
	};

	struct [[gnu::packed]]  PakHeader {
		constexpr static const char VALID_MAGIC[16] = "Europa Packfile";

		char magic[16]; // "Europa Packfile\0"

		/**
		 * Header size. Doesn't include the magic.
		 */
		u16 headerSize;

		PakVersion version;
		u8 pad;

		u32 tocOffset;

		// Dunno what this is
		u32 unk;

		u32 fileCount;

		// Could be Windows FILETIME?
		u32 unk2;

		u32 reservedPad;

		/**
		 * Get the real header size (including the magic).
		 */
		[[nodiscard]] constexpr std::size_t RealHeaderSize() const {
			return sizeof(magic) + static_cast<std::size_t>(headerSize);
		}
	};

	// A Toc entry (without string. Needs to be read in seperately)
	struct [[gnu::packed]] PakTocEntry {
		u32 offset;
		u32 size;

		// Seems to be the same as the header's
		// unk2?
		u32 unk3;
	};


	static_assert(sizeof(PakHeader) == 0x29, "PakHeader wrong size!!");
	static_assert(sizeof(PakTocEntry) == 0xc, "PakTocEntry wrong size!");

}

#endif // EUROPA_STRUCTS_PAK_H
