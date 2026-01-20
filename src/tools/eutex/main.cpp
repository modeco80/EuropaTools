//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <EutexConfig.hpp>
#include <toollib/ToolCommand.hpp>
#include <toollib/ToolMain.hpp>

int main(int argc, char** argv) {
	const tool::ToolInfo info {
		.name = "eutex",
		.version = EUTEX_VERSION_STR,
		.description = "Europa Tex (YATF) Tool v" EUTEX_VERSION_STR
	};

	auto toolCommands = std::vector {
		tool::ToolCommandFactory::CreateNamed("eutex_dump")
	};

	// clang-format off
    return tool::ToolMain(info, {
        .toolCommands = toolCommands,
        .argc = argc,
        .argv = argv
    });
	// clang-format on
}
