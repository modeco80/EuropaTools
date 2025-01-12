//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <europa/io/PakReader.hpp>
#include <fstream>
#include <iostream>
#include <tasks/InfoTask.hpp>
#include <Utils.hpp>

#include "europa/structs/Pak.hpp"

namespace eupak::tasks {

	constexpr static auto DATE_FORMAT = "%m/%d/%Y %r";

	int InfoTask::Run(Arguments&& args) {
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

} // namespace eupak::tasks