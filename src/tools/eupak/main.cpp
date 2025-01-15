//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <argparse/argparse.hpp>
#include <EupakConfig.hpp>
#include <tasks/CreateTask.hpp>
#include <tasks/ExtractTask.hpp>
#include <tasks/InfoTask.hpp>

#include "europa/structs/Pak.hpp"
#include "tasks/Task.hpp"

int main(int argc, char** argv) {
	// FIXME: At some point we should just have task classes register their arguments
	// and then they will deal with all that themselves. The only thing we will do here
	// is point them to themselves.
	//
	// The current design is really really bad for introducing new stuff.

	argparse::ArgumentParser parser("eupak", EUPAK_VERSION_STR);
	parser.add_description("Eupak (Europa Package Multi-Tool) v" EUPAK_VERSION_STR);

	argparse::ArgumentParser infoParser("info", EUPAK_VERSION_STR, argparse::default_arguments::help);
	infoParser.add_description("Print information about a package file.");
	infoParser.add_argument("input")
	.help("Input archive")
	.metavar("ARCHIVE");

	infoParser.add_argument("--verbose")
	.help("Increase information output verbosity (print a list of files).")
	.default_value(false)
	.implicit_value(true);

	argparse::ArgumentParser extractParser("extract", EUPAK_VERSION_STR, argparse::default_arguments::help);
	extractParser.add_description("Extract a package file.");
	extractParser.add_argument("-d", "--directory")
	.default_value("")
	.metavar("DIRECTORY")
	.help("Directory to extract to.");
	extractParser.add_argument("input")
	.help("Input archive")
	.metavar("ARCHIVE");

	extractParser.add_argument("--verbose")
	.help("Increase extraction output verbosity")
	.default_value(false)
	.implicit_value(true);

	parser.add_subparser(infoParser);
	parser.add_subparser(extractParser);

	auto tasks = std::vector {
		eupak::tasks::TaskFactory::CreateNamed("create", parser)
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

#if 0

	if(parser.is_subcommand_used("extract")) {
		eupak::tasks::ExtractTask task;
		eupak::tasks::ExtractTask::Arguments args;

		args.verbose = extractParser.get<bool>("--verbose");
		args.inputPath = eupak::fs::path(extractParser.get("input"));

		if(extractParser.is_used("--directory")) {
			args.outputDirectory = eupak::fs::path(extractParser.get("--directory"));
		} else {
			// Default to the basename appended to current path
			// as a "relatively sane" default path to extract to.
			// Should be okay.
			args.outputDirectory = eupak::fs::current_path() / args.inputPath.stem();
		}

		std::cout << "Input PAK/PMDL: " << args.inputPath << '\n';
		std::cout << "Output Directory: " << args.outputDirectory << '\n';

		return task.Run(std::move(args));
	}

	if(parser.is_subcommand_used("info")) {
		eupak::tasks::InfoTask task;
		eupak::tasks::InfoTask::Arguments args;

		args.verbose = infoParser.get<bool>("--verbose");
		args.inputPath = eupak::fs::path(infoParser.get("input"));

		return task.Run(std::move(args));
	}

	
	if(parser.is_subcommand_used("create")) {
		eupak::tasks::CreateTask task;
		eupak::tasks::CreateTask::Arguments args;

		
		return task.Run(std::move(args));
	}
#endif

	return 0;
}
