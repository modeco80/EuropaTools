//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include "StreamUtils.h"

#include <cstdint>
#include <europa/base/VirtualFileSystem.hpp>
#include <stdexcept>

namespace europa::io::impl {

	namespace detail {

		void ReadStreamTypeImpl(base::VfsFileHandle& is, char* buffer, std::size_t buffer_size) {
			if(auto n = is->Read(reinterpret_cast<std::uint8_t*>(buffer), buffer_size); n != buffer_size) {
				throw std::runtime_error("incomplete read");
			}
		}

		void WriteStreamTypeImpl(base::VfsFileHandle& is, const char* buffer, std::size_t buffer_size) {
			if(auto n = is->Write(reinterpret_cast<const std::uint8_t*>(buffer), buffer_size); n != buffer_size) {
				throw std::runtime_error("incomplete read");
			}
		}

	} // namespace detail

	std::string ReadZeroTerminatedString(base::VfsFileHandle& is) {
		std::string s;
		char c;

		while(true) {
			if(auto n = is->Read(reinterpret_cast<std::uint8_t*>(&c), 1); n == 1) {
				throw std::runtime_error("end of stream while trying to read asciiz string");
			}

			if(c == '\0')
				return s;

			s.push_back(c);
		}
	}

	std::string ReadPString(base::VfsFileHandle& is) {
		std::string s;

		std::uint32_t length = static_cast<std::uint32_t>(is->Get());

		if(length == 0) {
			return "";
		}

		// Read the string.
		s.resize(length - 1);
		if(auto n = is->Read(reinterpret_cast<std::uint8_t*>(&s[0]), length - 1); n != length - 1) {
			throw std::runtime_error("end of stream while trying to read asciip string");
		}
		static_cast<void>(is->Get());

		return s;
	}

	void WritePString(base::VfsFileHandle& os, const std::string& string) {
		if(string.length() >= 255)
			throw std::invalid_argument("String length too long for WritePString()");

		auto len = static_cast<std::uint8_t>(string.length() + 1);

		// Write the length and the string.
		if(os->Write(reinterpret_cast<std::uint8_t*>(&len), sizeof(len)) != sizeof(len)) {
			// short write, probably means we can't write anmymore
			return;
		}

		// It might be iffy to just depend on the C++ STL's
		// implicit NUL termination?
		// I mean, c_str() would be broken and the library implementation could be considered
		// faulty, so idk.
		os->Write(reinterpret_cast<const std::uint8_t*>(string.data()), string.length() + 1);
	}

	void TeeInOut(base::VfsFileHandle& is, base::VfsFileHandle& os) {
		std::uint8_t buffer[1024] {};
		int rc = 0;
		while(true) {
			rc = is->Read(&buffer[0], sizeof(buffer));
			if(rc == 0) {
				// end of stream
				break;
			}
			os->Write(&buffer[0], rc);
		}
	}

} // namespace europa::io::impl
