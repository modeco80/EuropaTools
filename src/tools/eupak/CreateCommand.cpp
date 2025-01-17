//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <chrono>

// Common stuff
#include <CommonDefs.hpp>
#include <EupakConfig.hpp>
#include <europa/io/pak/File.hpp>
#include <europa/io/pak/Writer.hpp>
#include <europa/io/pak/WriterProgressReportSink.hpp>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <toollib/ToolCommand.hpp>
#include <Utils.hpp>

#include "europa/structs/Pak.hpp"

namespace eupak {

	struct CreateArchiveReportSink : public eio::pak::WriterProgressReportSink {
		CreateArchiveReportSink(int fileCount = 0)
			: eio::pak::WriterProgressReportSink() {
			indicators::show_console_cursor(false);
			progress.set_option(indicators::option::MaxProgress { fileCount });
		}

		~CreateArchiveReportSink() {
			indicators::show_console_cursor(true);
		}

		void OnEvent(const PakEvent& event) override {
			using enum PakEvent::EventCode;
			switch(event.eventCode) {
				case WritingHeader:
					progress.set_option(indicators::option::PostfixText { "Writing header" });
					progress.print_progress();
					break;

				case FillInHeader:
					progress.set_option(indicators::option::PostfixText { "Filling in header" });
					progress.print_progress();
					break;

				case WritingToc:
					progress.set_option(indicators::option::PostfixText { "Writing TOC" });
					progress.print_progress();
					break;
			}
		}

		void OnEvent(const FileEvent& event) override {
			using enum FileEvent::EventCode;
			switch(event.eventCode) {
				case FileWriteBegin:
					progress.set_option(indicators::option::PostfixText { "Writing " + event.targetFileName });
					progress.print_progress();
					break;

				case FileWriteEnd:
					progress.set_option(indicators::option::PostfixText { "Written " + event.targetFileName });
					progress.tick();
					break;
			}
		}

	   private:
		indicators::ProgressBar progress {
			indicators::option::BarWidth { 50 },
			indicators::option::ForegroundColor { indicators::Color::yellow },
			indicators::option::ShowPercentage { true },
			indicators::option::ShowElapsedTime { true },
			indicators::option::ShowRemainingTime { true },

			indicators::option::PrefixText { "Writing archive " }
		};
	};

	std::optional<estructs::PakVersion> ParsePakVersion(const std::string& str) {
		if(str == "europa-prerelease") {
			return estructs::PakVersion::Ver3;
		} else if(str == "starfighter") {
			return estructs::PakVersion::Ver4;
		} else if(str == "jedistarfighter") {
			return estructs::PakVersion::Ver5;
		}

		return std::nullopt;
	}

	struct CreateCommand : tool::IToolCommand {
		CreateCommand()
			: parser("create", EUPAK_VERSION_STR, argparse::default_arguments::help) {
			// Setup argparse
			// clang-format off
			parser.add_description("Create a package file.");
			parser.add_argument("-d", "--directory")
				.required()
				.metavar("DIRECTORY")
				.help("Directory to create archive from");

			parser.add_argument("-V", "--archive-version")
				.default_value("starfighter")
				.help(R"(Output archive version. Either "europa-prerelease", "starfighter" or "jedistarfighter".)")
				.metavar("VERSION");

			parser.add_argument("-s", "--sector-aligned")
				.help(R"(Aligns all files in this new package to CD-ROM sector boundaries. Only valid for -V jedistarfighter.)")
				.flag();

			parser.add_argument("output")
				.required()
				.help("Output archive")
				.metavar("ARCHIVE");

			parser.add_argument("--verbose")
				.help("Increase creation output verbosity")
				.default_value(false)
				.implicit_value(true);

			// FIXME: At some point for bit-accurate rebuilds we should also accept a JSON manifest file
			// that contains: 
			// 	- Package version, 
			//	- sector alignment (for v5), 
			//	- package build time, 
			//	- data order of all files
			//	- TOC order of all files
			//	- file TOC data (modtime, TOC index, so on)
			// Then a user can just do `eupak create --manifest manifest.json` and it'll all be done for them
			//
			// `eupak extract` should optionally generate this manifest for the user
			// (I have not dreamt up the schema for this yet and this relies on other FIXMEs being done so this will have to wait.)

			// clang-format on
		}

		void Init(argparse::ArgumentParser& parentParser) override {
			parentParser.add_subparser(parser);
		}

		bool ShouldRun(argparse::ArgumentParser& parentParser) const override {
			return parentParser.is_subcommand_used("create");
		}

		int Parse() override {
			currentArgs.verbose = parser.get<bool>("--verbose");
			currentArgs.inputDirectory = fs::path(parser.get("--directory"));
			currentArgs.outputFile = fs::path(parser.get("output"));

			if(parser.is_used("--archive-version")) {
				const auto& versionStr = parser.get("--archive-version");

				if(auto opt = ParsePakVersion(versionStr); opt.has_value()) {
					currentArgs.pakVersion = *opt;
				} else {
					std::cout << "Error: Invalid version \"" << versionStr << "\"\n"
							  << parser;
					return 1;
				}
			} else {
				currentArgs.pakVersion = estructs::PakVersion::Ver4;
			}

			currentArgs.sectorAligned = parser.get<bool>("--sector-aligned");

			if(currentArgs.sectorAligned && currentArgs.pakVersion != estructs::PakVersion::Ver5) {
				std::cout << "Error: --sector-aligned is only valid for creating a package with \"-V jedistarfighter\".\n"
						  << parser;
				return 1;
			}

			if(!eupak::fs::is_directory(currentArgs.inputDirectory)) {
				std::cout << "Error: Provided input isn't a directory\n"
						  << parser;
				return 1;
			}

			return 0;
		}

		int Run() override {
			auto currFile = 0;
			auto fileCount = 0;

			// Count how many files we're gonna add to the archive
			for(auto& ent : fs::recursive_directory_iterator(currentArgs.inputDirectory)) {
				if(ent.is_directory())
					continue;
				fileCount++;
			}

			std::cout << "Going to write " << fileCount << " files into " << currentArgs.outputFile << '\n';

			if(currentArgs.sectorAligned) {
				std::cout << "Writing a sector aligned package\n";
			}

			indicators::ProgressBar progress {
				indicators::option::BarWidth { 50 },
				indicators::option::ForegroundColor { indicators::Color::green },
				indicators::option::MaxProgress { fileCount },
				indicators::option::ShowPercentage { true },
				indicators::option::ShowElapsedTime { true },
				indicators::option::ShowRemainingTime { true },

				indicators::option::PrefixText { "Adding files to archive " }
			};

			indicators::show_console_cursor(false);

			// TODO: use time to write in the header
			//			also: is there any point to verbosity? could add archive written size ig

			std::vector<eio::pak::Writer::FlattenedType> files;
			files.reserve(fileCount);

			for(auto& ent : fs::recursive_directory_iterator(currentArgs.inputDirectory)) {
				if(ent.is_directory())
					continue;

				auto relativePathName = fs::relative(ent.path(), currentArgs.inputDirectory).string();
				auto lastModified = fs::last_write_time(ent.path());

				// Convert to Windows path separator always (that's what the game wants, after all)
				for(auto& c : relativePathName)
					if(c == '/')
						c = '\\';

				progress.set_option(indicators::option::PostfixText { relativePathName + " (" + std::to_string(currFile + 1) + '/' + std::to_string(fileCount) + ")" });

				eio::pak::File file;
				eio::pak::FileData pakData = eio::pak::FileData::InitAsPath(ent.path());

				file.InitAs(currentArgs.pakVersion, currentArgs.sectorAligned);

				// Add data
				file.SetData(std::move(pakData));

				// Setup other stuff like modtime
				file.VisitTocEntry([&](auto& tocEntry) {
					// Kinda stupid but works
					auto sys = std::chrono::file_clock::to_sys(lastModified);
					auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(sys);
					tocEntry.creationUnixTime = static_cast<std::uint32_t>(seconds.time_since_epoch().count());
				});

				files.emplace_back(std::make_pair(relativePathName, std::move(file)));
				progress.tick();
				currFile++;
			}

			indicators::show_console_cursor(true);

			std::ofstream ofs(currentArgs.outputFile.string(), std::ofstream::binary);

			if(!ofs) {
				std::cout << "Error: Couldn't open " << currentArgs.outputFile << " for writing\n";
				return 1;
			}

			CreateArchiveReportSink reportSink(fileCount);
			eio::pak::Writer writer(currentArgs.pakVersion);

			using enum eio::pak::Writer::SectorAlignment;

			eio::pak::Writer::SectorAlignment alignment = DoNotAlign;

			if(currentArgs.sectorAligned)
				alignment = Align;

			writer.Write(ofs, std::move(files), reportSink, alignment);
			return 0;
		}

	   private:
		struct Arguments {
			fs::path inputDirectory;
			fs::path outputFile;

			bool verbose;
			europa::structs::PakVersion pakVersion;
			bool sectorAligned;
		};

		argparse::ArgumentParser parser;
		Arguments currentArgs;
	};

	TOOLLIB_REGISTER_TOOLCOMMAND("eupak_create", CreateCommand);

} // namespace eupak