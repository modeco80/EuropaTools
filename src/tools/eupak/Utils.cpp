//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

// MinGW bodges are cool.
#if defined(_WIN32) && !defined(_MSC_VER)
	#define _POSIX_THREAD_SAFE_FUNCTIONS
#endif

#include <Utils.hpp>
#include <algorithm>

namespace eupak {

	std::string FormatUnit(std::uint64_t bytes) {
		char buf[1024];
		constexpr auto unit = 1024;

		std::size_t exp {};
		std::size_t div = unit;

		if(bytes < unit) {
			sprintf(buf, "%zu B", bytes);
			return buf;
		} else {
			for(std::uint64_t i = bytes / unit; i >= unit; i /= unit) {
				// Break out if we're gonna set the exponent too high
				if((exp + 1) > 2)
					break;

				div *= unit;

				exp++; // TODO: break if too big
			}
		}

#define CHECKED_LIT(literal, expression) (literal)[std::clamp(expression, std::size_t(0), sizeof(literal) - 1)]
		sprintf(buf, "%0.2f %cB", float(bytes) / float(div), CHECKED_LIT("kMG", exp));
#undef CHECKED_LIT
		return buf;
	}

	std::string FormatUnixTimestamp(std::time_t time, const std::string_view format) {
		char buf[1024]{};
		tm tmObject{};

		localtime_r(&time, &tmObject);

		auto count = std::strftime(&buf[0], sizeof(buf), format.data(), &tmObject);

		// an error occured, probably.
		if(count == -1)
			return "";

		return { buf, count };
	}

}