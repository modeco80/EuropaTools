//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_EUPAK_UTILS_HPP
#define EUROPA_EUPAK_UTILS_HPP

#include <cstdint>
#include <ctime>
#include <string>

namespace eupak {

	/**
	 * Format a raw amount of bytes to a human-readable unit, if possible.
	 * \param[in] bytes Size in bytes.
	 */
	std::string formatUnit(std::uint64_t bytes);

	/**
	 * Formats a Unix timestamp using the strftime() C function.
	 *
	 * \param[in] time The Unix timestamp time to format
	 * \param[in] format The format string
	 * \return A formatted string corresponding to user input.
	 */
	std::string formatUnixTimestamp(std::time_t time, const std::string_view format);

} // namespace eupak

#endif // EUROPA_EUPAK_UTILS_HPP
