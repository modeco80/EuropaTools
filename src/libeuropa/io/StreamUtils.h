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

namespace europa::io {

	// This is lame. But it works :)
	template <class T>
	T LameRead(std::istream& is) {
		if(!is)
			throw std::runtime_error("stream is bad");

		T t {};
		is.read(reinterpret_cast<char*>(&t), sizeof(T));
		return t;
	}

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


}

#endif // EUROPA_TOOLS_STREAMUTILS_H
