//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

// Test/excercise of new toollib system.
#include <toollib/ToolCommand.hpp>
#include <toollib/ToolMain.hpp>

int main(int argc, char** argv) {
	auto tasks = std::vector {
		tool::ToolCommandFactory::CreateNamed("test")
	};

	// clang-format off
    return tool::ToolMain(tool::ToolInfo {
        .name = "toollib_test",
        .version = "0.0.1",
        .description = "A test/excercise of the new toollib APIs."
    }, tool::ToolMainInput {
        .toolCommands = tasks,
        .argc = argc,
        .argv = argv
    });
	// clang-format on
}