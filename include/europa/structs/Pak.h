//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_STRUCTS_PAK_H
#define EUROPA_STRUCTS_PAK_H

#include <europa/structs/ImHexAdapter.h>

#include <cstdint>
#include <cstring>

namespace europa::structs {

	enum class PakVersion : u16 {
		Starfighter = 0x4,
		Ver2 = 0x5
	};

	struct [[gnu::packed]] PakHeader {
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

		/**
		 * Initialize this header (used when writing).
		 */
		void Init(PakVersion ver) noexcept {
			// clear any junk
			memset(this, 0, sizeof(PakHeader));

			// Copy important things.
			std::memcpy(&magic[0], &VALID_MAGIC[0], sizeof(VALID_MAGIC));

			// Set proper header size.
			headerSize = sizeof(PakHeader) - (sizeof(PakHeader::VALID_MAGIC) - 1);

			// Set archive version
			version = ver;
		}

		[[nodiscard]] bool Valid() const noexcept {
			// Magic must match.
			if(std::strcmp(magic, VALID_MAGIC) != 0)
				return false;

			using enum PakVersion;

			// Version must match ones we support,
			// otherwise it's invalid.
			switch(version) {
				case Starfighter:
				case Ver2:
					break;

				default:
					return false;
			}

			// Header is okay.
			return true;
		}
	};

	// A Toc entry (without string. Needs to be read in separately)
	struct [[gnu::packed]] PakTocEntry {
		u32 offset;
		u32 size;

		// Seems to be the same as the header's
		// unk2?
		u32 unk3;
	};

	static_assert(sizeof(PakHeader) == 0x29, "PakHeader wrong size!!");
	static_assert(sizeof(PakHeader) - (sizeof(PakHeader::VALID_MAGIC) - 1) == 0x1a, "PakHeader::headerSize will be invalid when writing archives.");
	static_assert(sizeof(PakTocEntry) == 0xc, "PakTocEntry wrong size!");

} // namespace europa::structs

#endif // EUROPA_STRUCTS_PAK_H
