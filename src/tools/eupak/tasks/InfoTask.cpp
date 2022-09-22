//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include <tasks/InfoTask.hpp>

#include <europa/io/PakReader.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <Utils.hpp>

namespace eupak::tasks {

	constexpr static auto DATE_FORMAT = "%m/%d/%Y %r";

	int InfoTask::Run(Arguments&& args) {
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

		std::string version = "Version 4 (Starfighter)";

		if(reader.GetHeader().version == europa::structs::PakHeader::Version::Ver5)
			version = "Version 5 (Jedi Starfighter)";

		std::cout << "Archive " << args.inputPath << ":\n";
		std::cout << "    Created: " << FormatUnixTimestamp(reader.GetHeader().creationUnixTime, DATE_FORMAT) << '\n';
		std::cout << "    Version: " << version << '\n';
		std::cout << "    Size: " << FormatUnit(reader.GetHeader().tocOffset + reader.GetHeader().tocSize) << '\n';
		std::cout << "    File Count: " << reader.GetHeader().fileCount << " files\n";

		// Print a detailed file list if verbose.
		if(args.verbose) {
			for(auto& [ filename, file ] : reader.GetFiles()) {
				std::cout << "File \"" << filename << "\":\n";
				std::cout << "    Created: " << FormatUnixTimestamp(file.GetTOCEntry().creationUnixTime, DATE_FORMAT) << '\n';
				std::cout << "    Size: " << FormatUnit(file.GetTOCEntry().size) << '\n';
			}
		}

		return 0;
	}

}