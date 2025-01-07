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
		constexpr static u32 TextureFlag_Unknown = 0x1;

		/**
		 * Texture does not have a palette
		 */
		constexpr static u32 TextureFlag_NoPalette = 0x30000;

		/**
		 * Texture uses alpha.
		 */
		constexpr static u32 TextureFlag_UsesAlpha = 0x1000000;

		constexpr static auto ValidMagic = util::FourCC<"YATF", std::endian::little>();

		u32 magic;

		u32 flags;

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
