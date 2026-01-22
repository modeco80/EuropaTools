//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

// Math types which are used by mesh I/O code.
// These are not general purpose and are only meant to be
// binary compatible.

#pragma once

namespace europa::structs {

#ifdef _MSC_VER
	#pragma pack(push, 1)
#endif

	struct [[gnu::packed]] Vec3f {
		float x;
		float y;
		float z;
	};

	struct [[gnu::packed]] Uvf {
		float u;
		float v;
	};

#ifdef _MSC_VER
	#pragma pack(pop)
#endif

	static_assert(sizeof(Vec3f) == 0xc, "Vec3f size is incorrect");
	static_assert(sizeof(Uvf) == 0x8, "Uvf size is incorrect");

} // namespace europa::structs