//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
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
	}


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

	std::string ReadZeroTerminatedString(std::istream& is);
	std::string ReadPString(std::istream& is);

} // namespace europa::io::impl

#endif // EUROPA_TOOLS_STREAMUTILS_H
