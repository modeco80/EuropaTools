//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <argparse/argparse.hpp>
#include <CommonDefs.hpp>
#include <EupakConfig.hpp>
#include <europa/io/pak/Reader.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <toollib/ToolCommand.hpp>
#include <Utils.hpp>

namespace eupak {

	constexpr static auto DATE_FORMAT = "%m/%d/%Y %r";

	struct InfoCommand : tool::IToolCommand {
		InfoCommand()
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

		void Init(argparse::ArgumentParser& parentParser) override {
			parentParser.add_subparser(parser);
		}

		bool ShouldRun(argparse::ArgumentParser& parentParser) const override {
			return parentParser.is_subcommand_used("info");
		}

		int Parse() override {
			try {
				currentArgs.verbose = parser.get<bool>("--verbose");
				currentArgs.inputPath = eupak::fs::path(parser.get("input"));

				if(fs::is_directory(currentArgs.inputPath)) {
					std::cout << "Error: " << currentArgs.inputPath << " appears to be a directory, not a file.\n";
					return 1;
				}

			} catch(...) {
				return 1;
			}

			return 0;
		}

		int Run() override {
			std::ifstream ifs(currentArgs.inputPath.string(), std::ifstream::binary);

			if(!ifs) {
				std::cout << "Error: Could not open file " << currentArgs.inputPath << ".\n";
				return 1;
			}

			eio::pak::Reader reader(ifs);

			reader.ReadHeaderAndTOC();

			if(reader.Invalid()) {
				std::cout << "Error: Invalid PAK/PMDL file " << currentArgs.inputPath << ".\n";
				return 1;
			}

			std::visit([&](auto& header) {
				std::string_view version = "???";

				// This is the best other than just duplicating the body for each pak version.. :(
				if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver3)
					version = "Version 3 (Starfighter/Europa pre-release, May-July 2000?)";
				else if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver4)
					version = "Version 4 (Starfighter)";
				else if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver5)
					version = "Version 5 (Jedi Starfighter)";

				std::cout << "Archive " << currentArgs.inputPath << ":\n";
				std::cout << "    Created: " << FormatUnixTimestamp(header.creationUnixTime, DATE_FORMAT) << '\n';
				std::cout << "    Version: " << version << '\n';
				std::cout << "    Size: " << FormatUnit(header.tocOffset + header.tocSize) << '\n';
				std::cout << "    File Count: " << header.fileCount << " files\n";
			},
					   reader.GetHeader());

			// Print a detailed file list if verbose.

			for(auto& [filename, file] : reader.GetFiles()) {
				if(currentArgs.verbose) {
					std::cout << "File \"" << filename << "\":\n";
					file.VisitTocEntry([&](auto& tocEntry) {
						std::cout << "    Created: " << FormatUnixTimestamp(tocEntry.creationUnixTime, DATE_FORMAT) << '\n';
						std::cout << "    Size: " << FormatUnit(tocEntry.size) << '\n';

						if constexpr(std::is_same_v<std::decay_t<decltype(tocEntry)>, estructs::PakHeader_V5::TocEntry_SectorAligned>) {
							std::cout << "    Start LBA (CD-ROM Sector): " << tocEntry.startLBA << '\n';
						}
					});
				} else {
					file.VisitTocEntry([&](auto& tocEntry) {
						std::printf("%16s %10s %8s", FormatUnixTimestamp(tocEntry.creationUnixTime, DATE_FORMAT).c_str(), FormatUnit(tocEntry.size).c_str(), filename.c_str());

						if constexpr(std::is_same_v<std::decay_t<decltype(tocEntry)>, estructs::PakHeader_V5::TocEntry_SectorAligned>) {
							std::printf(" (LBA %u)", tocEntry.startLBA);
						}

						std::printf("\n");
					});
				}
			}

			return 0;
		}

	   private:
		struct Arguments {
			fs::path inputPath;
			bool verbose;
		};

		argparse::ArgumentParser parser;
		Arguments currentArgs;
	};

	TOOLLIB_REGISTER_TOOLCOMMAND("eupak_info", InfoCommand);

} // namespace eupak