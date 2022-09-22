//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include <europa/io/PakWriter.hpp>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <tasks/CreateTask.hpp>
#include <Utils.hpp>

namespace eupak::tasks {

	int CreateTask::Run(Arguments&& args) {
		europa::io::PakWriter writer;

		writer.Init(args.pakVersion);

		auto currFile = 0;
		auto fileCount = 0;

		// Count how many files we're gonna add to the archive
		for(auto& ent : fs::recursive_directory_iterator(args.inputDirectory)) {
			if(ent.is_directory())
				continue;
			fileCount++;
		}

		std::cout << "Going to write " << fileCount << " files into " << args.outputFile << '\n';

		indicators::ProgressBar progress {
			indicators::option::BarWidth { 50 },
			indicators::option::ForegroundColor { indicators::Color::green },
			indicators::option::MaxProgress { fileCount },
			indicators::option::ShowPercentage { true },
			indicators::option::ShowElapsedTime { true },
			indicators::option::ShowRemainingTime { true },

			indicators::option::PrefixText { "Creating archive " }
		};

		indicators::show_console_cursor(false);

		// TODO: use time to write in the header
		//			also: is there any point to verbosity? could add archive written size ig

		for(auto& ent : fs::recursive_directory_iterator(args.inputDirectory)) {
			if(ent.is_directory())
				continue;

			auto relativePathName = fs::relative(ent.path(), args.inputDirectory).string();
			auto lastModified = fs::last_write_time(ent.path());

			// Convert to Windows path separator always (that's what the game wants, after all)
			for(auto& c : relativePathName)
				if(c == '/')
					c = '\\';


			progress.set_option(indicators::option::PostfixText { relativePathName + " (" + std::to_string(currFile + 1) + '/' + std::to_string(fileCount) + ")"});

			std::ifstream ifs(ent.path(), std::ifstream::binary);

			if(!ifs) {
				std::cout << "Error: Couldn't open file for archive path \"" << relativePathName << "\"\n";
				return 1;
			}

			europa::io::PakFile file;
			europa::io::PakFile::DataType pakData;

			ifs.seekg(0, std::ifstream::end);
			pakData.resize(ifs.tellg());
			ifs.seekg(0, std::ifstream::beg);

			ifs.read(reinterpret_cast<char*>(&pakData[0]), pakData.size());

			file.SetData(std::move(pakData));
			file.FillTOCEntry();

			file.GetTOCEntry().creationUnixTime = static_cast<std::uint32_t>(lastModified.time_since_epoch().count());

			writer.GetFiles()[relativePathName] = std::move(file);

			progress.tick();
			currFile++;
		}

		std::ofstream ofs(args.outputFile.string(), std::ofstream::binary);

		if(!ofs) {
			std::cout << "Error: Couldn't open " << args.outputFile << " for writing\n";
			return 1;
		}

		writer.Write(ofs);
		indicators::show_console_cursor(true);
		return 0;
	}

} // namespace eupak::tasks