//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include <europa/io/PakReader.h>

#include <filesystem>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " [path to Europa PAK file]";
		return 1;
	}

	std::ifstream ifs(argv[1], std::ifstream::binary);

	if(!ifs) {
		std::cout << "Invalid file \"" << argv[1] << "\"\n";
		return 1;
	}

	europa::io::PakReader reader(ifs);

	auto baseDirectory = fs::path(argv[1]).stem();

	reader.ReadData();

	if(reader.Invalid()) {
		std::cout << "Invalid pak data in file \"" << argv[1] << "\"\n";
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

		auto outpath = (baseDirectory / nameCopy);

		if(!fs::exists(outpath.parent_path()))
			fs::create_directories(outpath.parent_path());

		reader.ReadFile(filename);

		std::ofstream ofs(outpath.string(), std::ofstream::binary);

		if(!ofs) {
			std::cerr << "Could not open \"" << outpath.string() << "\" for writing.\n";
			continue;
		}

		ofs.write(reinterpret_cast<const char*>(file.GetData().data()), static_cast<std::streampos>(file.GetTOCEntry().size));
		progress.tick();
	}

	indicators::show_console_cursor(true);
	return 0;
}