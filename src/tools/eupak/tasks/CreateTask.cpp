//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <chrono>
#include <europa/io/PakWriter.hpp>
#include <fstream>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <tasks/CreateTask.hpp>
#include <Utils.hpp>

#include "europa/io/PakFile.hpp"

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

	int CreateTask::Run(Arguments&& args) {
		auto currFile = 0;
		auto fileCount = 0;

		// Count how many files we're gonna add to the archive
		for(auto& ent : fs::recursive_directory_iterator(args.inputDirectory)) {
			if(ent.is_directory())
				continue;
			fileCount++;
		}

		std::cout << "Going to write " << fileCount << " files into " << args.outputFile << '\n';

		if(args.sectorAligned) {
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

			progress.set_option(indicators::option::PostfixText { relativePathName + " (" + std::to_string(currFile + 1) + '/' + std::to_string(fileCount) + ")" });

			eio::PakFile file;
			eio::PakFile::DataType pakData = eio::PakFileData::InitAsPath(ent.path());

			file.InitAs(args.pakVersion);

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

		std::ofstream ofs(args.outputFile.string(), std::ofstream::binary);

		if(!ofs) {
			std::cout << "Error: Couldn't open " << args.outputFile << " for writing\n";
			return 1;
		}

		CreateArchiveReportSink reportSink(fileCount);
		eio::PakWriter writer(args.pakVersion);

		using enum eio::PakWriter::SectorAlignment;

		eio::PakWriter::SectorAlignment alignment = DoNotAlign;

		if(args.sectorAligned)
			alignment = Align;

		writer.Write(ofs, std::move(files), reportSink, alignment);
		return 0;
	}

} // namespace eupak::tasks