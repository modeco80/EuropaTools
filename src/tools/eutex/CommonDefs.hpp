//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#include <filesystem>

namespace europa {
	namespace base {}
	namespace io {}
	namespace util {}
	namespace structs {}
} // namespace europa

namespace eutex {

	namespace fs = std::filesystem;

	// Shorthands for libeuropa components
	// that we might want to reference
	namespace ebase = europa::base;
	namespace eio = europa::io;
	namespace eutil = europa::util;
	namespace estructs = europa::structs;

} // namespace eutex