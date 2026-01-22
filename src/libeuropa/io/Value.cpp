//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <europa/io/Value.hpp>
#include <format>
#include <stdexcept>

#include "StreamUtils.h"

namespace europa::io {

	structs::Value readValue(mco::Stream& stream) {
		auto type = impl::ReadStreamType<structs::ValueType>(stream);

		// Based on the value type, read the appropiate value.
		using enum structs::ValueType;
		switch(type) {
			case String:
				return impl::ReadZeroTerminatedString(stream);
				break;
#define X(ValueT, CxxT) \
	case ValueT:        \
		return impl::ReadStreamType<CxxT>(stream);
				EUROPA_VALUE_TYPE_MAPPING_TRIVIAL()
#undef X
			default:
				throw std::runtime_error(std::format("Unhandled CValue Type 0x{:02x}.", static_cast<u8>(type)));
		}
	}

	void writeValue(const structs::Value& value, mco::WritableStream& stream) {
		// TODO: A better way to do this might involve switching on the type index.
		// For now, this works.
		std::visit([&](auto& val) {
			if constexpr(std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
				impl::WriteStreamType(stream, structs::ValueType::String);
				impl::writeNullTerminatedString(stream, val);
			}
#define X(Value, CxxT)                                                \
	if constexpr(std::is_same_v<std::decay_t<decltype(val)>, CxxT>) { \
		impl::WriteStreamType(stream, structs::ValueType::Value);     \
		impl::WriteStreamType(stream, val);                           \
	}
			EUROPA_VALUE_TYPE_MAPPING_TRIVIAL()
#undef X
		},
				   value);
	}
} // namespace europa::io
