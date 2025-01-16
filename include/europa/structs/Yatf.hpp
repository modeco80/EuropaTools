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
			kTextureFormat8Bpp = 0,
			kTextureFormatUnknown = 1, // possibly 16bpp?
			kTextureFormat24Bpp = 2,
			kTextureFormat32Bpp = 3
		};

		constexpr static auto ValidMagic = util::FourCC<"YATF", std::endian::big>();

		u32 magic;

		u16 unkThing; // always 0x1

		TextureFormat format;

		u8 unkThing2; // flags?

		// Always zeroed.
		u32 zero;

		u32 height;
		u32 width;

		[[nodiscard]] constexpr bool IsValid() const {
			return magic == ValidMagic;
		}
	};

} // namespace europa::structs

#endif // EUROPA_STRUCTS_YATF_H
