//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <CommonDefs.hpp>
#include <EupakConfig.hpp>
#include <europa/io/pak/Reader.hpp>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <stdexcept>
#include <toollib/ToolCommand.hpp>

namespace eupak {

	struct ExtractCommand : tool::IToolCommand {
		ExtractCommand()
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

		void Init(argparse::ArgumentParser& parentParser) override {
			parentParser.add_subparser(parser);
		}

		bool ShouldRun(argparse::ArgumentParser& parentParser) const override {
			return parentParser.is_subcommand_used("extract");
		}

		int Parse() override {
			currentArgs.verbose = parser.get<bool>("--verbose");
			currentArgs.inputPath = eupak::fs::path(parser.get("input"));

			if(parser.is_used("--directory")) {
				currentArgs.outputDirectory = eupak::fs::path(parser.get("--directory"));
			} else {
				// Default to the basename appended to current path
				// as a "relatively sane" default path to extract to.
				// Should be okay.
				currentArgs.outputDirectory = eupak::fs::current_path() / currentArgs.inputPath.stem();
			}

			return 0;
		}

		int Run() override {
			std::cout << "Input PAK/PMDL: " << currentArgs.inputPath << '\n';
			std::cout << "Output Directory: " << currentArgs.outputDirectory << '\n';

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

				auto outpath = (currentArgs.outputDirectory / nameCopy);

				if(!fs::exists(outpath.parent_path()))
					fs::create_directories(outpath.parent_path());

				std::ofstream ofs(outpath.string(), std::ofstream::binary);

				if(!ofs) {
					std::cerr << "Could not open " << outpath << " for writing.\n";
					continue;
				}

				if(currentArgs.verbose) {
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

	   private:
		struct Arguments {
			fs::path inputPath;
			fs::path outputDirectory;
			bool verbose;
		};

		argparse::ArgumentParser parser;
		Arguments currentArgs;
	};

	TOOLLIB_REGISTER_TOOLCOMMAND("eupak_extract", ExtractCommand);
} // namespace eupak