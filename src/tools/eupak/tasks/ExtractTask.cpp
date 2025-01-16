//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <EupakConfig.hpp>
#include <europa/io/pak/Reader.hpp>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <stdexcept>
#include <tasks/ExtractTask.hpp>

#include "tasks/Task.hpp"

namespace eupak::tasks {
	ExtractTask::ExtractTask()
		: parser("extract", EUPAK_VERSION_STR, argparse::default_arguments::help) {
		// clang-format off
		parser
			.add_description("Extract a package file.");
		parser
			.add_argument("-d", "--directory")
			.default_value("")
			.metavar("DIRECTORY")
			.help("Directory to extract to.");

		parser
			.add_argument("input")
			.help("Input archive")
			.metavar("ARCHIVE");

		parser
			.add_argument("--verbose")
			.help("Increase extraction output verbosity")
			.default_value(false)
			.implicit_value(true);
		// clang-format on
	}

	void ExtractTask::Init(argparse::ArgumentParser& parentParser) {
		parentParser.add_subparser(parser);
	}

	bool ExtractTask::ShouldRun(argparse::ArgumentParser& parentParser) const {
		return parentParser.is_subcommand_used("extract");
	};

	int ExtractTask::Parse() {
		eupak::tasks::ExtractTask task;
		eupak::tasks::ExtractTask::Arguments args;

		args.verbose = parser.get<bool>("--verbose");
		args.inputPath = eupak::fs::path(parser.get("input"));

		if(parser.is_used("--directory")) {
			args.outputDirectory = eupak::fs::path(parser.get("--directory"));
		} else {
			// Default to the basename appended to current path
			// as a "relatively sane" default path to extract to.
			// Should be okay.
			args.outputDirectory = eupak::fs::current_path() / args.inputPath.stem();
		}

		return 0;
	}

	int ExtractTask::Run() {
		const auto& args = currentArgs;

		std::cout << "Input PAK/PMDL: " << args.inputPath << '\n';
		std::cout << "Output Directory: " << args.outputDirectory << '\n';

		std::ifstream ifs(args.inputPath.string(), std::ifstream::binary);

		if(!ifs) {
			std::cout << "Error: Could not open file " << args.inputPath << ".\n";
			return 1;
		}

		eio::pak::Reader reader(ifs);

		reader.ReadHeaderAndTOC();

		if(reader.Invalid()) {
			std::cout << "Error: Invalid PAK/PMDL file " << args.inputPath << ".\n";
			return 1;
		}

		indicators::ProgressBar progress {
			indicators::option::BarWidth { 50 },
			indicators::option::ForegroundColor { indicators::Color::green },
			indicators::option::MaxProgress { reader.GetFiles().size() },
			indicators::option::ShowPercentage { true },
			indicators::option::ShowElapsedTime { true },
			indicators::option::ShowRemainingTime { true },

			indicators::option::PrefixText { "Extracting archive " }
		};

		indicators::show_console_cursor(false);

		for(auto& [filename, file] : reader.GetFiles()) {
			auto nameCopy = filename;

#ifndef _WIN32
			if(nameCopy.find('\\') != std::string::npos) {
				// Grody, but eh. Should work.
				for(auto& c : nameCopy)
					if(c == '\\')
						c = '/';
			}
#endif

			progress.set_option(indicators::option::PostfixText { filename });

			auto outpath = (args.outputDirectory / nameCopy);

			if(!fs::exists(outpath.parent_path()))
				fs::create_directories(outpath.parent_path());

			std::ofstream ofs(outpath.string(), std::ofstream::binary);

			if(!ofs) {
				std::cerr << "Could not open " << outpath << " for writing.\n";
				continue;
			}

			if(args.verbose) {
				std::cerr << "Extracting file \"" << filename << "\"...\n";
			}

			reader.ReadFile(filename);

			{
				auto& fileData = file.GetData();
				if(auto* buffer = fileData.GetIf<std::vector<std::uint8_t>>(); buffer) {
					ofs.write(reinterpret_cast<const char*>((*buffer).data()), (*buffer).size());
					ofs.flush();
				} else {
					throw std::runtime_error("???? why are we getting paths here?");
				}
			}

			// We no longer need the file data anymore, so let's purge it to save memory
			file.PurgeData();

			progress.tick();
		}

		indicators::show_console_cursor(true);
		return 0;
	}

	EUPAK_REGISTER_TASK("extract", ExtractTask);
} // namespace eupak::tasks