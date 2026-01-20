//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_FOURCC_H
#define EUROPA_FOURCC_H

#include <bit>
#include <europa/util/FixedString.hpp>

namespace europa::util {

	/// Compile-time endian-safe FourCC.
	template <FixedString fccString, std::endian Endian = std::endian::little>
	consteval std::uint32_t FourCC() {
		// FIXME: It may be useful to *optionally* add policy support
		// some FourCC clients prefer '\0' padding, some prefer ' ' (0x20) padding
		// Idk. Pretty useless here though so idk
		static_assert(fccString.Length() == 4, "Provided string is not a FourCC");

		switch(Endian) {
			case std::endian::little:
				return (fccString[0]) | (fccString[1] << 8) | (fccString[2] << 16) | (fccString[3] << 24);

			case std::endian::big:
				return (fccString[0] << 24) | (fccString[1] << 16) | (fccString[2] << 8) | fccString[3];
		}

		// If the user provided an invalid case, do something which is
		// constexpr-unsafe to indicate user error.
		throw 0xffffffff; // You passed an invalid Endian to FourCC()?
	}

} // namespace europa::util

#endif // EUROPA_FOURCC_H
