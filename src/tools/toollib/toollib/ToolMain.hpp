//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#include <memory>
#include <span>
#include <string_view>

namespace tool {

	struct IToolCommand;

	struct ToolInfo {
		std::string_view name;		  // "Eupak"
		std::string_view version;	  // v1.0.0
		std::string_view description; // "bla"
									  // FIXME: (authors?)
	};

	struct ToolMainInput {
		/// Tool commands to run.
		std::span<std::shared_ptr<IToolCommand>> toolCommands;

		// C arguments
		int argc;
		char** argv;
	};

	/// The shared toollib main. When in doubt, use this.
	int ToolMain(const ToolInfo& toolInfo, const ToolMainInput& mainInput);

} // namespace tool