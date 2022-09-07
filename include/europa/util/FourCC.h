//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_FOURCC_H
#define EUROPA_FOURCC_H

#include <europa/util/FixedString.h>

#include <bit>

namespace europa::util {

	/**
	 * A multi-endian, compile-time FourCC generator.
	 * You love to see it.
	 */
	template <FixedString fccString, std::endian Endian = std::endian::little>
	consteval std::uint32_t FourCC() {
		static_assert(fccString.Length() == 4, "Provided string is not a FourCC");

		switch(Endian) {
			case std::endian::little:
				return (fccString[0]) | (fccString[1] << 8) | (fccString[2] << 16) | (fccString[3] << 24);

			case std::endian::big:
				return (fccString[0] << 24) | (fccString[1] << 16) | (fccString[2] << 8) | fccString[3];
		}

		// Just return something with all possible bits set, if the user somehow
		// got around the above switch (which shouldn't happen).
		return 0xffffffff;
	}

} // namespace europa::util

#endif // EUROPA_FOURCC_H
