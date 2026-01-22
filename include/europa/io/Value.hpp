//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_VALUE_HPP
#define EUROPA_IO_VALUE_HPP

#include <cstdint>
#include <europa/structs/Value.hpp>
#include <europa/util/Overloaded.hpp>
#include <mco/io/stream.hpp>

namespace europa::io {

	/// Reads a single serialized Europa CValue from [stream].
	structs::Value readValue(mco::Stream& stream);

	/// Writes a serialized Europa CValue of [value] to [stream].
	void writeValue(const structs::Value& value, mco::WritableStream& stream);

} // namespace europa::io

#endif
