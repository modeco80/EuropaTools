//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_C3PO_CLASSINFO_HPP
#define EUROPA_IO_C3PO_CLASSINFO_HPP

#include <mco/base_types.hpp>
#include <string>
#include <vector>

namespace europa::io::c3po {

	struct ClassInfo {
		using VariableInfo = std::tuple<std::string, u8, structs::Value>;
		virtual ~ClassInfo() = default;

		std::string name;
		std::string extends;
		std::vector<VariableInfo> properties;
	};

} // namespace europa::io::c3po

#endif
