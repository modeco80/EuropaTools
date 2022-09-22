//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//


// MinGW bodges are cool.
#if defined(_WIN32) && !defined(_MSC_VER)
	#define _POSIX_THREAD_SAFE_FUNCTIONS
#endif

#include <tasks/InfoTask.hpp>

#include <europa/io/PakReader.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace eupak::tasks {

	namespace {
		/**
 	 	 * Format a raw amount of bytes to a human-readable unit.
 	 	 * \param[in] bytes Size in bytes.
		 */
		std::string FormatUnit(std::uint64_t bytes) {
			char buf[1024];
			constexpr auto unit = 1024;

			std::size_t exp {};
			std::size_t div = unit;

			if(bytes < unit) {
				sprintf(buf, "%zu B", bytes);
				return buf;
			} else {
				for(std::uint64_t i = bytes / unit; i >= unit; i /= unit) {
					div *= unit;
					exp++; // TODO: break if too big
				}
			}

#define CHECKED_LIT(literal, expression) (literal)[std::clamp(expression, std::size_t(0), sizeof(literal) - 1)]
			sprintf(buf, "%0.2f %cB", float(bytes) / float(div), CHECKED_LIT("kMG", exp));
#undef CHECKED_LIT
			return buf;
		}

		std::string FormatUnixTimestamp(time_t time, const std::string_view format) {
			char buf[1024]{};
			tm tmObject{};

			localtime_r(&time, &tmObject);

			auto count = std::strftime(&buf[0], sizeof(buf), format.data(), &tmObject);

			// an error occured, probably.
			if(count == -1)
				return "";

			return { buf, count };
		}
	}

	constexpr static auto DATE_FORMAT = "%m/%d/%Y %r";

	int InfoTask::Run(InfoTask::Arguments&& args) {
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