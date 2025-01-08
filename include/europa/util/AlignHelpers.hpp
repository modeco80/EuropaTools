//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_UTIL_ALIGNHELPERS_HPP
#define EUROPA_UTIL_ALIGNHELPERS_HPP

#include <cstddef>

namespace europa::util {

    /// Aligns a integral (e.g: file offset) to the provided value.
	template <class T>
	constexpr T AlignBy(T value, std::size_t alignment) {
		return static_cast<T>(((value + (alignment - 1)) & ~(alignment - 1)));
	}

} // namespace europa::util

#endif