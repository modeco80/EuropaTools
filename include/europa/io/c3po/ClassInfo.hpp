//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_C3PO_CLASSINFO_HPP
#define EUROPA_IO_C3PO_CLASSINFO_HPP

#include <string>
#include <vector>

namespace europa::io::c3po {

	struct ClassBaseInfo {
		virtual ~ClassBaseInfo() = default;
		std::string c
	}

	struct ClassInfo : public ClassBaseInfo {
	};

	struct WeaponInfo : public ClassBaseInfo {
	}

} // namespace europa::io::c3po

#endif
