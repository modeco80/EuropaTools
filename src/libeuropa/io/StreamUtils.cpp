//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include "StreamUtils.h"

#include <cstdint>
#include <stdexcept>

namespace europa::io::impl {

	namespace detail {

		void ReadStreamTypeImpl(mco::Stream& is, char* buffer, usize size) {
			if(auto n = is.read(&buffer[0], size); n != size) {
				throw std::runtime_error("Short/incomplete read!");
			}
		}

		void WriteStreamTypeImpl(mco::WritableStream& os, const char* buffer, usize buffer_size) {
			if(auto n = os.write(&buffer[0], buffer_size); n != buffer_size) {
				throw std::runtime_error("Short/incomplete write!");
			}
		}

	} // namespace detail

	std::string ReadZeroTerminatedString(mco::Stream& is) {
		std::string s;
		char c;

		while(true) {
			c = static_cast<char>(is.get());

			if(c == '\0')
				return s;

			s.push_back(c);
		}
	}

	std::string ReadPString(mco::Stream& is) {
		std::string s;
		u32 length = static_cast<u32>(is.get());

		if(length == 0) {
			// TODO: is this even possible/valid?
			static_cast<void>(is.get());
			return "";
		}

		// Read the string.
		s.resize(length - 1);
		is.read(&s[0], length - 1);
		static_cast<void>(is.get());

		return s;
	}

	void writeNullTerminatedString(mco::WritableStream& os, const std::string& string) {
		if(auto n = os.write(string.data(), string.length() + 1); n != string.length() + 1) {
			throw std::runtime_error("Short/incomplete write in writeNullTerminatedString()!");
		}
	}

	void WritePString(mco::WritableStream& os, const std::string& string) {
		if(string.length() > 254)
			throw std::invalid_argument("String length too long for WritePString()");
		// Write the length and the string.
		os.put(static_cast<std::uint8_t>(string.length() + 1));
		writeNullTerminatedString(os, string);
	}

} // namespace europa::io::impl
