//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <EupakConfig.hpp>
#include <europa/io/PakReader.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tasks/InfoTask.hpp>
#include <Utils.hpp>

#include "argparse/argparse.hpp"
#include "europa/structs/Pak.hpp"

namespace eupak::tasks {

	constexpr static auto DATE_FORMAT = "%m/%d/%Y %r";

	InfoTask::InfoTask()
		: parser("info", EUPAK_VERSION_STR, argparse::default_arguments::help) {
		// clang-format off
		parser
			.add_description("Print information about a package file.");
		parser
			.add_argument("input")
			.help("Input archive")
			.metavar("ARCHIVE");

		// FIXME: Probably just print this always, in a thinner format, but use
		// the existing thicker format for verbosity.
		parser
			.add_argument("--verbose")
			.help("Increase information output verbosity (print a list of files).")
			.default_value(false)
			.implicit_value(true);
		// clang-format on
	}

	void InfoTask::Init(argparse::ArgumentParser& parentParser) {
		parentParser.add_subparser(parser);
	}

	int InfoTask::Parse() {
		auto& args = currentArgs;

		try {
			args.verbose = parser.get<bool>("--verbose");
			args.inputPath = eupak::fs::path(parser.get("input"));

			if(fs::is_directory(args.inputPath)) {
				std::cout << "Error: " << args.inputPath << " appears to be a directory, not a file.\n";
				return 1;
			}

		} catch(...) {
			return 1;
		}

		return 0;
	}

	bool InfoTask::ShouldRun(argparse::ArgumentParser& parentParser) const {
		return parentParser.is_subcommand_used("info");
	}

	int InfoTask::Run() {
		const auto& args = currentArgs;

		std::ifstream ifs(args.inputPath.string(), std::ifstream::binary);

		if(!ifs) {
			std::cout << "Error: Could not open file " << args.inputPath << ".\n";
			return 1;
		}

		eio::PakReader reader(ifs);

		reader.ReadHeaderAndTOC();

		if(reader.Invalid()) {
			std::cout << "Error: Invalid PAK/PMDL file " << args.inputPath << ".\n";
			return 1;
		}

		std::visit([&](auto& header) {
			std::string_view version = "???";

			// This is the best other than just duplicating the body for each pak version.. :(
			if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver3)
				version = "Version 3 (PMDL)";
			else if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver4)
				version = "Version 4 (Starfighter)";
			else if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver5)
				version = "Version 5 (Jedi Starfighter)";

			std::cout << "Archive " << args.inputPath << ":\n";
			std::cout << "    Created: " << FormatUnixTimestamp(header.creationUnixTime, DATE_FORMAT) << '\n';
			std::cout << "    Version: " << version << '\n';
			std::cout << "    Size: " << FormatUnit(header.tocOffset + header.tocSize) << '\n';
			std::cout << "    File Count: " << header.fileCount << " files\n";
		},
				   reader.GetHeader());

		// Print a detailed file list if verbose.
		if(args.verbose) {
			for(auto& [filename, file] : reader.GetFiles()) {
				std::cout << "File \"" << filename << "\":\n";
				file.VisitTocEntry([&](auto& tocEntry) {
					std::cout << "    Created: " << FormatUnixTimestamp(tocEntry.creationUnixTime, DATE_FORMAT) << '\n';
					std::cout << "    Size: " << FormatUnit(tocEntry.size) << '\n';

					if constexpr(std::is_same_v<std::decay_t<decltype(tocEntry)>, estructs::PakHeader_V5::TocEntry_SectorAligned>) {
						std::cout << "    Start LBA (CD-ROM Sector): " << tocEntry.startLBA << '\n';
					}
				});
			}
		}

		return 0;
	}

	EUPAK_REGISTER_TASK("info", InfoTask);

} // namespace eupak::tasks