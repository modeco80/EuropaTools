//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_UTIL_FIXEDSTRING_H
#define EUROPA_UTIL_FIXEDSTRING_H

#include <cstdint>

namespace europa::util {

	/**
	 * A compile-time string. Usable as a C++20 cNTTP.
	 */
	template <std::size_t N>
	struct FixedString {
		char buf[N + 1] {};

		constexpr FixedString(const char* s) { // NOLINT
			for(unsigned i = 0; i != N; ++i)
				buf[i] = s[i];
		}

		constexpr operator const char*() const { // NOLINT
			return buf;
		}

		[[nodiscard]] constexpr std::size_t Length() const {
			return N;
		}
	};

	template <std::size_t N>
	FixedString(char const (&)[N]) -> FixedString<N - 1>;

} // namespace europa::util

#endif // EUROPA_UTIL_FIXEDSTRING_H
