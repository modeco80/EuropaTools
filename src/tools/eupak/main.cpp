//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <argparse/argparse.hpp>
#include <EupakConfig.hpp>
#include <tasks/Task.hpp>

int main(int argc, char** argv) {
	argparse::ArgumentParser parser("eupak", EUPAK_VERSION_STR);
	parser.add_description("Eupak (Europa Package Multi-Tool) v" EUPAK_VERSION_STR);

	auto tasks = std::vector {
		eupak::tasks::TaskFactory::CreateNamed("create", parser),
		eupak::tasks::TaskFactory::CreateNamed("info", parser),
		eupak::tasks::TaskFactory::CreateNamed("extract", parser),
	};

	try {
		// No command was specified, display the help and then exit with a failure code.
		// For some reason the `argparse` library does not have something like this on its own.
		//
		// I guess it's simple though so I can't really complain that much
		if(argc == 1) {
			auto s = parser.help();
			printf("%s\n", s.str().c_str());
			return 1;
		}

		parser.parse_args(argc, argv);
	} catch(std::runtime_error& error) {
		std::cout << error.what() << '\n'
				  << parser;
		return 1;
	}

	// Run the given task

	for(auto task : tasks) {
		if(task->ShouldRun(parser)) {
			if(auto res = task->Parse(); res != 0)
				return res;

			return task->Run();
		}
	}

	return 0;
}
