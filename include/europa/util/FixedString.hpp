//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_UTIL_FIXEDSTRING_H
#define EUROPA_UTIL_FIXEDSTRING_H

#include <mco/base_types.hpp>

namespace europa::util {

	/// Compile-time fixed string.
	template <usize N>
	struct FixedString {
		char buf[N + 1] {};

		constexpr FixedString() = default;

		constexpr FixedString(const char* pString) {
			for(usize i = 0; i < N; ++i)
				buf[i] = pString[i];
		}

		constexpr FixedString(const char* pString, usize length) {
			for(usize i = 0; i < N; ++i)
				buf[i] = pString[i];
		}

		constexpr FixedString(const char* pString, usize length, char padCharacter) {
			// copy string
			for(usize i = 0; i < length; ++i)
				buf[i] = pString[i];

			// pad string
			if(length < N) {
				for(usize i = length; i < N; ++i)
					buf[i] = padCharacter;
			}
		}

		[[nodiscard]] constexpr operator const char*() const {
			return buf;
		}

		[[nodiscard]] constexpr usize length() const {
			return N;
		}

		/// Truncate this string to a new length.
		template <usize newLength>
		consteval auto truncate() const {
			if(newLength < N) {
				// string is larger, truncate it
				return FixedString<newLength>(this->buf, N);
			} else {
				// string is smaller. than desired length, pad with zeroes.
				return FixedString<newLength>(this->buf, length());
			}
		}
	};

	template <usize N>
	FixedString(char const (&)[N]) -> FixedString<N - 1>;

} // namespace europa::util

#endif // EUROPA_UTIL_FIXEDSTRING_H
