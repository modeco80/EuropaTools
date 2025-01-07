//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_TOOLS_STREAMUTILS_H
#define EUROPA_TOOLS_STREAMUTILS_H

#include <iostream>
#include <string>

namespace europa::io::impl {

	namespace detail {
		void ReadStreamTypeImpl(std::istream& is, char* buffer, std::size_t size);
		void WriteStreamTypeImpl(std::ostream& os, const char* buffer, std::size_t buffer_size);
	} // namespace detail

	// This is lame. But it works :)
	template <class T>
	constexpr T ReadStreamType(std::istream& is) {
		T object {};

		// Absolutely UB.
		union Hack {
			T* t;
			char* c;
		} address {
			.t = &object
		};

		detail::ReadStreamTypeImpl(is, address.c, sizeof(T));

		return object;
	}

	template <class T>
	constexpr void WriteStreamType(std::ostream& os, const T& object) {
		// Absolutely UB.
		union Hack {
			const T* t;
			const char* c;
		} address {
			.t = &object
		};

		detail::WriteStreamTypeImpl(os, address.c, sizeof(T));
	}

	std::string ReadZeroTerminatedString(std::istream& is);
	std::string ReadPString(std::istream& is);

	void WritePString(std::ostream& os, const std::string& string);

	/// Tees a input stream to an output stream until the input stream signals EOF.
	void TeeInOut(std::istream& is, std::ostream& os);

} // namespace europa::io::impl

#endif // EUROPA_TOOLS_STREAMUTILS_H
