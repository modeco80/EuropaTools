//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <EupakConfig.hpp>
#include <toollib/ToolCommand.hpp>
#include <toollib/ToolMain.hpp>

int main(int argc, char** argv) {
	const tool::ToolInfo info {
		.name = "eupak",
		.version = EUPAK_VERSION_STR,
		.description = "Europa Package Tool v" EUPAK_VERSION_STR
	};

	auto toolCommands = std::vector {
		tool::ToolCommandFactory::CreateNamed("eupak_create"),
		tool::ToolCommandFactory::CreateNamed("eupak_extract"),
		tool::ToolCommandFactory::CreateNamed("eupak_info"),
	};

	// clang-format off
    return tool::ToolMain(info, {
        .toolCommands = toolCommands,
        .argc = argc,
        .argv = argv
    });
	// clang-format on
}
