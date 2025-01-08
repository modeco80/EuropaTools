//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include "StreamUtils.h"

#include <cstdint>
#include <stdexcept>

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

		if(!is)
			return "";

		std::uint32_t length = static_cast<std::uint32_t>(is.get());

		if(length == 0) {
			static_cast<void>(is.get());
			return "";
		}

		s.resize(length - 1);

		// Read the string
		for(std::uint32_t i = 0; i < length-1; ++i) {
			s[i] = static_cast<char>(is.get());
		}
		static_cast<void>(is.get());
		return s;
	}

	void WritePString(std::ostream& os, const std::string& string) {
		if(string.length() >= 255)
			throw std::invalid_argument("String length too long for WritePString()");

		auto len = static_cast<std::uint8_t>(string.length() + 1);

		// Write the length and the string.
		os.write(reinterpret_cast<char*>(&len), sizeof(len));
		
		// It might be iffy to just depend on the C++ STL's
		// implicit NUL termination? 
		// I mean, c_str() would be broken and the library implementation could be considered 
		// faulty, so idk.
		os.write(string.data(), string.length() + 1);
	}

	void TeeInOut(std::istream& is, std::ostream& os) {
		std::uint8_t buffer[1024] {};
		//int i = 0;

		while(!is.eof()) {
			is.read(reinterpret_cast<char*>(&buffer[0]), sizeof(buffer));
			auto c = is.gcount();
			//fprintf(stderr, "loop %d: Read %d bytes\n", i, c);
			os.write(reinterpret_cast<char*>(&buffer[0]), c);
			//i++;
		}
	}

} // namespace europa::io::impl
