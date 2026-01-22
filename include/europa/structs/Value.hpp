//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_STRUCTS_VALUE_HPP
#define EUROPA_STRUCTS_VALUE_HPP

#include <cstdio>
#include <cstring>
#include <mco/base_types.hpp>
#include <string>
#include <variant>

namespace europa::structs {

#define EUROPA_VALUE_TYPE_MAPPING() \
	X(String, std::string)          \
	X(Bool, bool)                   \
	X(Char, i8)                     \
	X(UChar, u8)                    \
	X(Short, i16)                   \
	X(UShort, u16)                  \
	X(Long, i32)                    \
	X(ULong, u32)                   \
	X(Float, float)                 \
	X(Double, double)

#define EUROPA_VALUE_TYPE_MAPPING_TRIVIAL() \
	X(Bool, bool)                           \
	X(Char, i8)                             \
	X(UChar, u8)                            \
	X(Short, i16)                           \
	X(UShort, u16)                          \
	X(Long, i32)                            \
	X(ULong, u32)                           \
	X(Float, float)                         \
	X(Double, double)

	/// The type of a CValue.
	enum class ValueType : u8 {
		Null = 0x0,
		String = 0x1,

		Bool = 0x3,

		Char = 0x4,
		UChar = 0x5,

		Short = 0x6,
		UShort = 0x7,

		Long = 0x8,
		ULong = 0x9,

		Float = 0xa,
		// the game has 0xb, but this can't ever
		// be read from a bin, so it's not useful.

		Double = 0xc, //? Just 8 bytes read.

		Error = 0xf,
	};

	// Uh oh.. here comes the template metaprogramming!
	// Don't worry. It's not too bad.
	namespace impl {
		template <class T>
		struct ValueTypeFromNativeType {
			constexpr static auto Type = ValueType::Null;
		};

#define X(ValType, CppType)                              \
	template <>                                          \
	struct ValueTypeFromNativeType<CppType> {            \
		constexpr static auto Type = ValueType::ValType; \
	};
		EUROPA_VALUE_TYPE_MAPPING()
#undef X

		template <class T>
		constexpr ValueType getValueTypeFor() {
			return ValueTypeFromNativeType<T>::Type;
		}
	} // namespace impl

	using impl::getValueTypeFor;

	/// A variant which can hold all CValue types.
	/// We use this instead of our own type because it's safer,
	/// and C++ provides this for us.
	using Value = std::variant<
	std::string,
	bool,
	i8,
	u8,
	i16,
	u16,
	i32,
	u32,
	float,
	double>;

} // namespace europa::structs

#endif
