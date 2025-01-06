//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include "StreamUtils.h"

#include <cstdint>

namespace europa::io::impl {

	namespace detail {

		void ReadStreamTypeImpl(std::istream& is, char* buffer, std::size_t size) {
			if(!is)
				throw std::runtime_error("stream is bad");

			is.read(&buffer[0], size);
		}

		void WriteStreamTypeImpl(std::ostream& os, const char* buffer, std::size_t buffer_size) {
			os.write(&buffer[0], buffer_size);
		}

	} // namespace detail

	std::string ReadZeroTerminatedString(std::istream& is) {
		std::string s;
		char c;

		if(!is)
			return "";

		while(true) {
			c = static_cast<char>(is.get());

			if(c == '\0')
				return s;

			s.push_back(c);
		}
	}

	std::string ReadPString(std::istream& is) {
		std::string s;
		char c;

		if(!is)
			return "";

		// should be just resizing, and refactor this loop to not do this,
		// but .... meh. I'll get to it if it's a problem
		std::uint8_t length = is.get();
		s.reserve(length);

		while(true) {
			c = static_cast<char>(is.get());

			if(c == '\0')
				return s;

			s.push_back(c);
		}
	}

	void TeeInOut(std::istream& is, std::ostream& os) {
		std::uint8_t buffer[4096] {};

		while(!is.eof()) {
			if(!is.read(reinterpret_cast<char*>(&buffer[0]), sizeof(buffer)))
				break;

			os.write(reinterpret_cast<char*>(&buffer[0]), is.gcount());
		}
	}

} // namespace europa::io::impl
