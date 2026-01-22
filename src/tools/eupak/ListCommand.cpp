//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <argparse/argparse.hpp>
#include <CommonDefs.hpp>
#include <EupakConfig.hpp>
#include <europa/io/pak/Reader.hpp>
#include <filesystem>
#include <iostream>
#include <mco/io/file_stream.hpp>
#include <toollib/ToolCommand.hpp>
#include <Utils.hpp>

namespace eupak {

	constexpr static auto DATE_FORMAT = "%m/%d/%Y %r";

	struct ListCommand : tool::IToolCommand {
		ListCommand()
			: parser("list", EUPAK_VERSION_STR, argparse::default_arguments::help) {
			// clang-format off
		parser
			.add_description("Lists package file information and files.");
		parser
			.add_argument("input")
			.help("Input archive")
			.metavar("ARCHIVE");

		// FIXME: Probably just print this always, in a thinner format, but use
		// the existing thicker format for verbosity.
		parser
			.add_argument("-v", "--verbose")
			.help("Increase information output verbosity.")
			.default_value(false)
			.implicit_value(true);
			// clang-format on
		}

		void init(argparse::ArgumentParser& parentParser) override {
			parentParser.add_subparser(parser);
		}

		bool shouldRun(argparse::ArgumentParser& parentParser) const override {
			return parentParser.is_subcommand_used("list");
		}

		int parse() override {
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

		int run() override {
			auto ifs = mco::FileStream::open(currentArgs.inputPath.string().c_str(), mco::FileStream::Read);
			eio::pak::Reader reader(ifs);

			// TODO: toollib should wrap run in a try/catch and or we should wrap this code in it ourselves.
			reader.init();

			std::visit([&](auto& header) {
				std::string_view version {};

				// This is the best other than just duplicating the body for each pak version.. :(
				// sucks but it is what it is
				if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver3)
					version = "Version 3 (Starfighter pre-release)";
				else if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver4)
					version = "Version 4 (Starfighter)";
				else if constexpr(std::decay_t<decltype(header)>::VERSION == estructs::PakVersion::Ver5)
					version = "Version 5 (Jedi Starfighter)";

				printf("Archive \"%s\": %s, created %s, %d files", currentArgs.inputPath.string().c_str(), version.data(), formatUnixTimestamp(header.creationUnixTime, DATE_FORMAT).c_str(), header.fileCount);

				if(currentArgs.verbose) {
					printf(", data size: %s", formatUnit(header.tocOffset + header.tocSize).c_str());
				}

				printf("\n");
			},
					   reader.getPackageHeader());

			for(const auto& [filename, file] : reader.getPackageFiles()) {
				file.visitTOCEntry([&](auto& tocEntry) {
					std::printf("%16s %10s %8s", formatUnixTimestamp(tocEntry.creationUnixTime, DATE_FORMAT).c_str(), formatUnit(tocEntry.size).c_str(), filename.c_str());

					if constexpr(std::is_same_v<std::decay_t<decltype(tocEntry)>, estructs::PakHeader_V5::TocEntry_SectorAligned>) {
						std::printf(" (LBA %u)", tocEntry.startLBA);
					}

					std::printf("\n");
				});
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

	TOOLLIB_REGISTER_TOOLCOMMAND("eupak_list", ListCommand);

} // namespace eupak
