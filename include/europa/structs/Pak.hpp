//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_STRUCTS_PAK_H
#define EUROPA_STRUCTS_PAK_H

#include <cstdio>
#include <cstring>
#include <europa/structs/ImHexAdapter.hpp>
#include <variant>

namespace europa::structs {

	constexpr static const char VALID_MAGIC[16] = "Europa Packfile";

	enum class PakVersion : u16 {
		Invalid = 0xffff,
		Ver3 = 0x3, ///< Early PMDL files use this version
		Ver4 = 0x4,
		Ver5 = 0x5
	};

	
#ifdef _MSC_VER
	#pragma pack(push, 1)
#endif

	struct [[gnu::packed]] PakHeader_Common {
		char magic[16]; // "Europa Packfile\0"

		/**
		 * Header size. Doesn't include the magic.
		 */
		u8 revision;
		u8 padding;

		PakVersion version;

		bool Valid() const {
			return !std::strcmp(magic, VALID_MAGIC);
		}
	};

	template <class Impl, PakVersion Version>
	struct [[gnu::packed]] PakHeader_Impl : PakHeader_Common {
		constexpr static auto VERSION = Version;

		constexpr static u16 HeaderSize() {
			return sizeof(Impl) - (sizeof(VALID_MAGIC) - 1);
		}

		PakHeader_Impl() {
			// clear any junk
			memset(this, 0, sizeof(PakHeader_Impl));

			version = Version;

			// Copy important things & set proper revision. I guess
			std::memcpy(&magic[0], &VALID_MAGIC[0], sizeof(VALID_MAGIC));
			revision = 0x1a;
		}

		explicit PakHeader_Impl(const PakHeader_Common& header) {
			memcpy(&magic[0], &header.magic[0], sizeof(header.magic));
			version = header.version;
			revision = header.revision;
		}

		[[nodiscard]] bool Valid() const noexcept {
			// Magic must match.
			if(!reinterpret_cast<const PakHeader_Common*>(this)->Valid())
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
			u16 junk; // always 0xff
			u32 creationUnixTime;
		};

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		u32 creationUnixTime;

		// Zeroes.
		u32 reservedPad {};
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

		struct [[gnu::packed]] TocEntry_SectorAligned {
			u32 offset;
			u32 size;
			// Start in LBA (offset / kCDSectorSize)
			u32 startLBA;
			u32 creationUnixTime;
		};

		u8 pad;

		u32 tocOffset;

		u32 tocSize;

		u32 fileCount;

		u32 creationUnixTime;

		// Zeroes.
		u32 reservedPad;

		// Seems to be the sector alignment value
		// Always 0x800
		u32 sectorAlignment;

		// 0x1 - This package is sector aligned
		// 0x0 - it is not
		u8 sectorAlignedFlag;
		u8 pad2;
	};

	
#ifdef _MSC_VER
	#pragma pack(pop)
#endif

	using PakHeaderVariant = std::variant<
	structs::PakHeader_V3,
	structs::PakHeader_V4,
	structs::PakHeader_V5>;

	using PakTocEntryVariant = std::variant<
	structs::PakHeader_V3::TocEntry,
	structs::PakHeader_V4::TocEntry,
	// PAK V5 has two kinds of toc entries:
	// - normal
	// - sector aligned
	structs::PakHeader_V5::TocEntry,
	structs::PakHeader_V5::TocEntry_SectorAligned>;

	// Make sure all the structures defined here match observed/expected sizes.
	// Otherwise, we will compile a broken program.

	static_assert(sizeof(PakHeader_V3) == 0x28, "PakHeader_V3 wrong size");
	static_assert(sizeof(PakHeader_V4) == 0x29, "PakHeader_V4 wrong size!!");
	static_assert(sizeof(PakHeader_V5) == 0x2f, "PakHeader_V5 wrong size!!");

	static_assert(sizeof(PakHeader_V3::TocEntry) == 0xe, "V3 TocEntry wrong size!");
	static_assert(sizeof(PakHeader_V4::TocEntry) == 0xc, "V4 TocEntry wrong size!");
	static_assert(sizeof(PakHeader_V5::TocEntry) == 0xc, "V5 TocEntry wrong size!");
	static_assert(sizeof(PakHeader_V5::TocEntry_SectorAligned) == 0x10, "V5 TocEntry_SectorAligned wrong size!");

} // namespace europa::structs

#endif // EUROPA_STRUCTS_PAK_H
