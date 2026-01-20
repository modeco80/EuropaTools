//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_TOOLS_STREAMUTILS_H
#define EUROPA_TOOLS_STREAMUTILS_H

#include <mco/io/stream.hpp>
#include <string>

namespace europa::io::impl {

	namespace detail {
		void ReadStreamTypeImpl(mco::Stream& is, char* buffer, std::size_t size);
		void WriteStreamTypeImpl(mco::WritableStream& os, const char* buffer, std::size_t buffer_size);
	} // namespace detail

	// This is lame. But it works :)
	template <class T>
	constexpr T ReadStreamType(mco::Stream& is) {
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
	constexpr void WriteStreamType(mco::WritableStream& os, const T& object) {
		// Absolutely UB.
		union Hack {
			const T* t;
			const char* c;
		} address {
			.t = &object
		};

		detail::WriteStreamTypeImpl(os, address.c, sizeof(T));
	}

	std::string ReadZeroTerminatedString(mco::Stream& is);
	std::string ReadPString(mco::Stream& is);

	void WritePString(mco::WritableStream& os, const std::string& string);

} // namespace europa::io::impl

#endif // EUROPA_TOOLS_STREAMUTILS_H
