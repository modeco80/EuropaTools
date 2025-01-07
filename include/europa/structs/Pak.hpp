//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_STRUCTS_PAK_H
#define EUROPA_STRUCTS_PAK_H

#include <cstdint>
#include <cstring>
#include <europa/structs/ImHexAdapter.hpp>
#include <optional>
#include <variant>

#include <cstdio>

namespace europa::structs {

	constexpr static const char VALID_MAGIC[16] = "Europa Packfile";

	enum class PakVersion : u16 {
		Invalid = 0xffff,
		Ver3 = 0x3, ///< Typically used for PMDL files
		Ver4 = 0x4,
		Ver5 = 0x5
	};

	struct [[gnu::packed]] PakHeader_Common {
		char magic[16]; // "Europa Packfile\0"

		/**
		 * Header size. Doesn't include the magic.
		 */
		u16 headerSize;

		PakVersion version;

		bool Valid() const {
			return !std::strcmp(magic, VALID_MAGIC);
		}
	};

	template <class Impl, PakVersion Version>
	struct [[gnu::packed]] PakHeader_Impl : PakHeader_Common {
		constexpr static auto VERSION = Version;

		/**
		 * Get the real header size (including the magic).
		 */
		[[nodiscard]] constexpr std::size_t RealHeaderSize() const {
			return sizeof(magic) + static_cast<std::size_t>(headerSize);
		}

		constexpr static u16 HeaderSize() {
			return sizeof(Impl) - (sizeof(VALID_MAGIC) - 1);
		}

		PakHeader_Impl() {
			// clear any junk
			memset(this, 0, sizeof(PakHeader_Impl));

			version = Version;

			// Copy important things & set proper header size.
			std::memcpy(&magic[0], &VALID_MAGIC[0], sizeof(VALID_MAGIC));
			headerSize = HeaderSize();
		}

		explicit PakHeader_Impl(const PakHeader_Common& header) {
			memcpy(&magic[0], &header.magic[0], sizeof(header.magic));
			version = header.version;
			headerSize = header.headerSize;
		}

		[[nodiscard]] bool Valid() const noexcept {
			// Magic must match.
			if(!reinterpret_cast<const PakHeader_Common*>(this)->Valid())
				return false;

			// Check header size.
			if(headerSize != HeaderSize() && headerSize != HeaderSize() + 1)
				return false;

			return version == Version;
		}
	};

	struct [[gnu::packed]] PakHeader_V3 : public PakHeader_Impl<PakHeader_V3, PakVersion::Ver3> {
		using PakHeader_Impl<PakHeader_V3, PakVersion::Ver3>::VERSION;
		using PakHeader_Impl<PakHeader_V3, PakVersion::Ver3>::PakHeader_Impl;
		using PakHeader_Impl<PakHeader_V3, PakVersion::Ver3>::Valid;

		struct [[gnu::packed]] TocEntry {
			u32 offset;
			u32 size;
			u32 creationUnixTime; // junk on these v3 files
			u16 junk;
		};

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		u32 creationUnixTime;

		// Zeroes.
		u32 reservedPad{};
	};

	struct [[gnu::packed]] PakHeader_V4 : public PakHeader_Impl<PakHeader_V4, PakVersion::Ver4> {
		using PakHeader_Impl<PakHeader_V4, PakVersion::Ver4>::PakHeader_Impl;

		struct [[gnu::packed]] TocEntry {
			u32 offset;
			u32 size;
			u32 creationUnixTime;
		};

		u8 pad;

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		u32 creationUnixTime;

		// Zeroes.
		u32 reservedPad;
	};

	struct [[gnu::packed]] PakHeader_V5 : public PakHeader_Impl<PakHeader_V5, PakVersion::Ver5> {
		using PakHeader_Impl<PakHeader_V5, PakVersion::Ver5>::PakHeader_Impl;

		struct [[gnu::packed]] TocEntry {
			u32 offset;
			u32 size;
			u32 creationUnixTime;
		};

		u8 pad;

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		u32 creationUnixTime;

		// Zeroes.
		u32 reservedPad;
	};

	using PakHeaderVariant = std::variant<
	structs::PakHeader_V3,
	structs::PakHeader_V4,
	structs::PakHeader_V5>;

	using PakTocEntryVariant = std::variant<
	structs::PakHeader_V3::TocEntry,
	structs::PakHeader_V4::TocEntry,
	structs::PakHeader_V5::TocEntry>;

	static_assert(sizeof(PakHeader_V3) == 0x28, "PakHeader_V3 wrong size");
	// TODO: their format really seems to be wrong, 0x19 is proper, but some v3 archives have 0x1a header size
	// ??? very weird
	//static_assert(sizeof(PakHeader_V3) - (sizeof(VALID_MAGIC) - 1) == 0x1a, "PakHeader_V3::headerSize will be invalid when writing archives.");
	static_assert(sizeof(PakHeader_V4) == 0x29, "PakHeader_V4 wrong size!!");
	static_assert(sizeof(PakHeader_V4) - (sizeof(VALID_MAGIC) - 1) == 0x1a, "PakHeader_V4::headerSize will be invalid when writing archives.");
	static_assert(sizeof(PakHeader_V5) == 0x29, "PakHeader_V5 wrong size!!");
	static_assert(sizeof(PakHeader_V5) - (sizeof(VALID_MAGIC) - 1) == 0x1a, "PakHeader_V5::headerSize will be invalid when writing archives.");

	static_assert(sizeof(PakHeader_V3::TocEntry) == 0xe, "V3 TocEntry wrong size!");
	static_assert(sizeof(PakHeader_V4::TocEntry) == 0xc, "V4 PakTocEntry wrong size!");
	static_assert(sizeof(PakHeader_V5::TocEntry) == 0xc, "V5 PakTocEntry wrong size!");

} // namespace europa::structs

#endif // EUROPA_STRUCTS_PAK_H
