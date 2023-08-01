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

	struct CreateArchiveReportSink : public europa::io::PakProgressReportSink {

		CreateArchiveReportSink(int fileCount = 0)
			: europa::io::PakProgressReportSink() {
			indicators::show_console_cursor(false);
			progress.set_option(indicators::option::MaxProgress { fileCount });
		}

		~CreateArchiveReportSink() {
			indicators::show_console_cursor(true);
		}

		void OnEvent(const PakEvent& event) override {
			using enum PakEvent::Type;
			switch(event.type) {
				case WritingHeader:
					progress.set_option(indicators::option::PostfixText {"Writing header"});
					progress.print_progress();
					break;

				case FillInHeader:
					progress.set_option(indicators::option::PostfixText {"Filling in header"});
					progress.print_progress();
					break;

				case WritingToc:
					progress.set_option(indicators::option::PostfixText {"Writing TOC"});
					progress.print_progress();
					break;
			}
		}

		void OnEvent(const FileEvent& event) override {
			using enum FileEvent::Type;
			switch(event.type) {
				case FileBeginWrite:
					progress.set_option(indicators::option::PostfixText {"Writing " + event.filename});
					progress.print_progress();
					break;

				case FileEndWrite:
					progress.set_option(indicators::option::PostfixText {"Written " + event.filename});
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

	int CreateTask::Run(Arguments&& args) {
		europa::io::PakWriter writer;

		writer.SetVersion(args.pakVersion);

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

			indicators::option::PrefixText { "Adding files to archive " }
		};

		indicators::show_console_cursor(false);

		// TODO: use time to write in the header
		//			also: is there any point to verbosity? could add archive written size ig

		std::vector<europa::io::PakWriter::FlattenedType> files;
		files.reserve(fileCount);

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

			file.InitAs(args.pakVersion);

			//file.GetTOCEntry().creationUnixTime = static_cast<std::uint32_t>(lastModified.time_since_epoch().count());

			files.emplace_back(std::make_pair(relativePathName, std::move(file)));
			progress.tick();
			currFile++;
		}

		indicators::show_console_cursor(true);

		std::ofstream ofs(args.outputFile.string(), std::ofstream::binary);

		if(!ofs) {
			std::cout << "Error: Couldn't open " << args.outputFile << " for writing\n";
			return 1;
		}

		CreateArchiveReportSink sink(fileCount);

		writer.Write(ofs, std::move(files), sink);
		return 0;
	}

} // namespace eupak::tasks