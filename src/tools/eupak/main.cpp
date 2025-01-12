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

	argparse::ArgumentParser createParser("create", EUPAK_VERSION_STR, argparse::default_arguments::help);
	createParser.add_description("Create a package file.");
	createParser.add_argument("-d", "--directory")
	.required()
	.metavar("DIRECTORY")
	.help("Directory to create archive from");

	createParser.add_argument("-V", "--archive-version")
	.default_value("starfighter")
	.help(R"(Output archive version. Either "pmdl", "starfighter" or "jedistarfighter".)")
	.metavar("VERSION");

	createParser.add_argument("-s", "--sector-aligned")
	.help(R"(Aligns all files in this new package to CD-ROM sector boundaries. Only valid for -V jedistarfighter.)")
	.flag();

	createParser.add_argument("output")
	.required()
	.help("Output archive")
	.metavar("ARCHIVE");

	createParser.add_argument("--verbose")
	.help("Increase creation output verbosity")
	.default_value(false)
	.implicit_value(true);

	parser.add_subparser(infoParser);
	parser.add_subparser(extractParser);
	parser.add_subparser(createParser);

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

	// FIXME: At some point for accurate rebuilds we should also accept a JSON manifest file
	// that contains: Package version, sector alignment, package build time, order of all files (as original) and their modtime, so on.
	// Then a user can just do `eupak create --manifest manifest.json` and it'll all be figured out
	// (I have not dreamt up the schema for this yet and this relies on other FIXMEs being done so this will have to wait.)
	if(parser.is_subcommand_used("create")) {
		eupak::tasks::CreateTask task;
		eupak::tasks::CreateTask::Arguments args;

		args.verbose = createParser.get<bool>("--verbose");
		args.inputDirectory = eupak::fs::path(createParser.get("--directory"));
		args.outputFile = eupak::fs::path(createParser.get("output"));

		if(createParser.is_used("--archive-version")) {
			const auto& versionStr = createParser.get("--archive-version");

			if(versionStr == "pmdl") {
				args.pakVersion = europa::structs::PakVersion::Ver3;
			} else if(versionStr == "starfighter") {
				args.pakVersion = europa::structs::PakVersion::Ver4;
			} else if(versionStr == "jedistarfighter") {
				args.pakVersion = europa::structs::PakVersion::Ver5;
			} else {
				std::cout << "Error: Invalid version \"" << versionStr << "\"\n"
						  << createParser;
				return 1;
			}
		} else {
			args.pakVersion = europa::structs::PakVersion::Ver4;
		}

		args.sectorAligned = createParser.get<bool>("--sector-aligned");

		if(args.sectorAligned && args.pakVersion != eupak::estructs::PakVersion::Ver5) {
			std::cout << "Error: --sector-aligned is only valid for creating a package with \"-V jedistarfighter\".\n"
					  << createParser;
			return 1;
		}

		if(!eupak::fs::is_directory(args.inputDirectory)) {
			std::cout << "Error: Provided input isn't a directory\n"
					  << createParser;
			return 1;
		}

		return task.Run(std::move(args));
	}

	return 0;
}
