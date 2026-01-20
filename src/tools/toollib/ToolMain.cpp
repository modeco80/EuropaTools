//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <toollib/ToolCommand.hpp>
#include <toollib/ToolMain.hpp>

namespace tool {
	int ToolMain(const ToolInfo& toolInfo, const ToolMainInput& mainInput) {
        // :( Again, FUCK argparse.
		argparse::ArgumentParser parser(std::string(toolInfo.name), std::string(toolInfo.version));
		parser.add_description(std::string(toolInfo.description));

        // Add commands to this parser
		for(auto& toolCmds : mainInput.toolCommands) {
            toolCmds->Init(parser);
        }

		try {
			// No command was specified, display the help and then exit with a failure code.
			// For some reason the `argparse` library does not have something like this on its own.
			//
			// I guess it's simple though so I can't really complain that much
			if(mainInput.argc == 1) {
				auto s = parser.help();
				printf("%s\n", s.str().c_str());
				return 1;
			}

			parser.parse_args(mainInput.argc, mainInput.argv);
		} catch(std::runtime_error& error) {
			std::cout << error.what() << '\n'
					  << parser;
			return 1;
		}

		for(auto& toolCmds : mainInput.toolCommands) {
			if(toolCmds->ShouldRun(parser)) {
				if(auto res = toolCmds->Parse(); res != 0)
					return res;

				return toolCmds->Run();
			}
		}

		return 0;
	}
} // namespace tool