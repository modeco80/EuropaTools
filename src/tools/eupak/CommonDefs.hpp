//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_EUPAK_COMMONDEFS_HPP
#define EUROPA_EUPAK_COMMONDEFS_HPP

#include <filesystem>

namespace europa {
	namespace base {}
	namespace io {}
	namespace util {}
	namespace structs {}
} // namespace europa

namespace eupak {

	namespace fs = std::filesystem;

	// Shorthands for libeuropa components
	namespace ebase = europa::base;
	namespace eio = europa::io;
	namespace eutil = europa::util;
	namespace estructs = europa::structs;

} // namespace eupak

#endif // EUROPA_EUPAK_COMMONDEFS_HPP
