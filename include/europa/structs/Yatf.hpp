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
#include <type_traits>

namespace europa::structs {

	struct [[gnu::packed]] YatfHeader {
		enum class TextureFormat : u8 {
			kTextureFormatV1_8Bpp = 0,
			kTextureFormatV1_24Bpp = 2,
			kTextureFormatV1_32Bpp = 3,

			// V2/jsf uses these
			kTextureFormatV2_8Bpp = 1,
			kTextureFormatV2_24Bpp = 3,
			kTextureFormatV2_32Bpp = 4,
			kTextureFormatV2_4Bpp = 5
		};

		// For some reason Jedi Starfighter YATFs use a different fourcc. ???
		constexpr static auto ValidMagicSF = util::FourCC<"YATF", std::endian::big>();
		constexpr static auto ValidMagicJSF = util::FourCC<"YATF", std::endian::little>();

		u32 magic;

		u16 version; // 0x1 for starfighter, 0x2 for jsf

		TextureFormat format;

		u8 unkThing2; // flags? some palbpp?

		// Always zeroed.
		u32 zero;

		u32 height;
		u32 width;

		[[nodiscard]] constexpr bool IsValid() const {
			return magic == ValidMagicSF || magic == ValidMagicJSF;
		}
	};

} // namespace europa::structs

#endif // EUROPA_STRUCTS_YATF_H
