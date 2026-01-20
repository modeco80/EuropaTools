//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
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

				exp++;
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

#ifdef _MSC_VER
		// All I will choose to say is: Why didn't Microsoft
		// just implement _r versions. This is nearly exactly
		// the same as those! ... This is why I hate cross-platform
		// software development sometimes
		localtime_s(&tmObject, &time);
#else
		localtime_r(&time, &tmObject);
#endif

		auto count = std::strftime(&buf[0], sizeof(buf), format.data(), &tmObject);

		// an error occured, probably.
		if(count == static_cast<std::size_t>(-1))
			return "";

		return { buf, count };
	}

}