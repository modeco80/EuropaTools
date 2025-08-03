//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_TOOLS_STREAMUTILS_H
#define EUROPA_TOOLS_STREAMUTILS_H

#include <europa/base/VirtualFileSystem.hpp>
#include <string>

namespace europa::io::impl {

	namespace detail {
		void ReadStreamTypeImpl(base::VfsFileHandle& is, char* buffer, std::size_t size);
		void WriteStreamTypeImpl(base::VfsFileHandle& os, const char* buffer, std::size_t buffer_size);
	} // namespace detail

	// This is lame. But it works :)
	template <class T>
	constexpr T ReadStreamType(base::VfsFileHandle& is) {
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
	constexpr void WriteStreamType(base::VfsFileHandle& os, const T& object) {
		// Absolutely UB.
		union Hack {
			const T* t;
			const char* c;
		} address {
			.t = &object
		};

		detail::WriteStreamTypeImpl(os, address.c, sizeof(T));
	}

	std::string ReadZeroTerminatedString(base::VfsFileHandle& is);
	std::string ReadPString(base::VfsFileHandle& is);

	void WritePString(base::VfsFileHandle& os, const std::string& string);

	/// Tees a input stream to an output stream until the input stream signals EOF.
	void TeeInOut(base::VfsFileHandle& is, base::VfsFileHandle& os);

} // namespace europa::io::impl

#endif // EUROPA_TOOLS_STREAMUTILS_H
