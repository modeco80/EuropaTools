//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include <tasks/ExtractTask.hpp>

#include <europa/io/PakReader.hpp>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>

// this actually is pretty fast so maybe I won't bother doing crazy thread optimizations..

namespace eupak::tasks {

	int ExtractTask::Run(Arguments&& args) {
		std::ifstream ifs(args.inputPath.string(), std::ifstream::binary);

		if(!ifs) {
			std::cout << "Error: Could not open file " << args.inputPath << ".\n";
			return 1;
		}

		europa::io::PakReader reader(ifs);

		reader.ReadData();

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

			reader.ReadFile(filename);

			std::ofstream ofs(outpath.string(), std::ofstream::binary);

			if(!ofs) {
				std::cerr << "Could not open " << outpath << " for writing.\n";
				continue;
			}

			if(args.verbose) {
				std::cerr << "Extracting file \"" << filename << "\"...\n";
			}

			ofs.write(reinterpret_cast<const char*>(file.GetData().data()), static_cast<std::streampos>(file.GetSize()));
			ofs.flush();
			progress.tick();
		}

		indicators::show_console_cursor(true);
		return 0;
	}

}