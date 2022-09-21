//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef EUROPA_STRUCTS_PAK_H
#define EUROPA_STRUCTS_PAK_H

#include <europa/structs/ImHexAdapter.h>

#include <cstdint>
#include <cstring>

namespace europa::structs {

	struct [[gnu::packed]] PakHeader {
		constexpr static const char VALID_MAGIC[16] = "Europa Packfile";

		enum class Version : u16 {
			Ver4 = 0x4,
			Ver5 = 0x5
		};

		char magic[16]; // "Europa Packfile\0"

		/**
		 * Header size. Doesn't include the magic.
		 */
		u16 headerSize;

		Version version;
		u8 pad;

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		u32 creationUnixTime;

		// Zeroes.
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
		void Init(Version ver) noexcept {
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

			// Check header size.
			if(headerSize != sizeof(PakHeader) - (sizeof(PakHeader::VALID_MAGIC) - 1))
				return false;

			using enum Version;

			// Version must match ones we support,
			// otherwise it's invalid.
			switch(version) {
				case Ver4:
				case Ver5:
					return true;

				default:
					return false;
			}
		}
	};

	// A Toc entry (without string. Needs to be read in separately)
	struct [[gnu::packed]] PakTocEntry {
		u32 offset;
		u32 size;
		u32 creationUnixTime;
	};


	static_assert(sizeof(PakHeader) == 0x29, "PakHeader wrong size!!");
	static_assert(sizeof(PakHeader) - (sizeof(PakHeader::VALID_MAGIC) - 1) == 0x1a, "PakHeader::headerSize will be invalid when writing archives.");
	static_assert(sizeof(PakTocEntry) == 0xc, "PakTocEntry wrong size!");

} // namespace europa::structs

#endif // EUROPA_STRUCTS_PAK_H
