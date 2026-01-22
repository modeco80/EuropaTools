//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

// laziness.. should probably remove this file

#ifndef EUROPA_STRUCTS_IMHEXADAPTER_H
#define EUROPA_STRUCTS_IMHEXADAPTER_H

#include <cstdint>

#error This file should not be included, use <mco/base_types.hpp>

namespace europa::structs {
	using u8 = std::uint8_t;
	using s8 = std::int8_t;

	using u16 = std::uint16_t;
	using s16 = std::int16_t;

	using u32 = std::uint32_t;
	using s32 = std::int32_t;

	using u64 = std::uint64_t;
	using s64 = std::int64_t;
} // namespace europa::structs

#endif // EUROPA_STRUCTS_IMHEXADAPTER_H
