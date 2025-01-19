//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_STRUCTS_YATF_H
#define EUROPA_STRUCTS_YATF_H

#include <europa/structs/ImHexAdapter.hpp>
#include <europa/util/FourCC.hpp>

namespace europa::structs {

#ifdef _MSC_VER
	#pragma pack(push, 1)
#endif

	struct [[gnu::packed]] YatfHeader {
		enum class TextureFormat : u8 {
			// V1 formats.
			kTextureFormatV1_8Bpp = 0,
			kTextureFormatV1_24Bpp = 2,
			kTextureFormatV1_32Bpp = 3,

			// V2/jsf uses these
			kTextureFormatV2_8Bpp = 1,
			kTextureFormatV2_24Bpp = 3,
			kTextureFormatV2_32Bpp = 4,
			kTextureFormatV2_4Bpp = 5
		};

		enum class Version : u16 {
			Invalid = 0xffff,
			Version1 = 1,
			Version2 = 2,
		};

		// For some reason Jedi Starfighter (V2) YATFs use a different fourcc endianness than V1.
		// When writing we should use the appropiate one.
		constexpr static auto ValidMagic_V1 = util::FourCC<"YATF", std::endian::big>();
		constexpr static auto ValidMagic_V2 = util::FourCC<"YATF", std::endian::little>();

		u32 magic;

		Version version;

		TextureFormat format;

		u8 unkThing2; // flags? some palbpp? V2 seems to end up usually matching

		// Always zeroed.
		u32 zero;

		u32 height;
		u32 width;

		[[nodiscard]] constexpr bool IsValid() const {
			if(auto magicValid = magic == ValidMagic_V1 || magic == ValidMagic_V2; !magicValid)
				return false;

			// Make sure version is sensical
			return version == Version::Version1 || version == Version::Version2;
		}
	};

#ifdef _MSC_VER
	#pragma pack(pop)
#endif

} // namespace europa::structs

#endif // EUROPA_STRUCTS_YATF_H
