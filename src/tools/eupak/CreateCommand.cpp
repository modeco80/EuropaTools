//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <chrono>

// Common stuff
#include <CommonDefs.hpp>
#include <EupakConfig.hpp>
#include <europa/io/pak/File.hpp>
#include <europa/io/pak/Writer.hpp>
#include <europa/io/pak/WriterProgressReportSink.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <mco/io/file_stream.hpp>
#include <toollib/ToolCommand.hpp>
#include <Utils.hpp>

#include "daw/json/daw_json_exception.h"
#include "europa/structs/Pak.hpp"
#include "ManifestJSON.hpp"

namespace eupak {

	struct CreateArchiveReportSink : public eio::pak::WriterProgressReportSink {
		CreateArchiveReportSink(int fileCount = 0)
			: eio::pak::WriterProgressReportSink() {
			indicators::show_console_cursor(false);
			progress.set_option(indicators::option::MaxProgress { fileCount });
		}

		~CreateArchiveReportSink() {
			indicators::show_console_cursor(true);
		}

		void onEvent(const PakEvent& event) override {
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

		void onEvent(const FileEvent& event) override {
			using enum FileEvent::EventCode;
			switch(event.eventCode) {
				case FileWriteBegin:
					progress.set_option(indicators::option::PostfixText { "Writing " + event.targetFileName });
					progress.print_progress();
					break;

				case FileWriteEnd:
					// ?
					// progress.set_option(indicators::option::PostfixText { "Written " + event.targetFileName });
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

	struct CreateCommand : tool::IToolCommand {
		CreateCommand()
			: parser("create", EUPAK_VERSION_STR, argparse::default_arguments::help) {
			// Setup argparse
			// clang-format off
			parser.add_description("Create a package file.");
			parser.add_argument("-m", "--manifest")
			.required()
			.metavar("MANIFEST")
			.help("Manifest to use to create archive from");

			parser.add_argument("output")
			.required()
			.help("Output archive")
			.metavar("ARCHIVE");

			parser.add_argument("--verbose")
			.help("Increase creation output verbosity")
			.default_value(false)
			.implicit_value(true);

			// clang-format on
		}

		void init(argparse::ArgumentParser& parentParser) override {
			parentParser.add_subparser(parser);
		}

		bool shouldRun(argparse::ArgumentParser& parentParser) const override {
			return parentParser.is_subcommand_used("create");
		}

		int parse() override {
			currentArgs.verbose = parser.get<bool>("--verbose");
			currentArgs.inputManifest = fs::path(parser.get("--manifest"));
			currentArgs.outputFile = fs::path(parser.get("output"));

			if(!eupak::fs::is_regular_file(currentArgs.inputManifest)) {
				std::cout << "Error: Provided manifest isn't a file or doesn't exist\n"
						  << parser;
				return 1;
			}

			// Parse the manifest JSON file.
			try {
				std::string buffer;
				// Read the entire manifest into the string
				// TODO: This could be a VFS utility function
				{
					auto n = 0;
					auto fh = mco::FileStream::open(currentArgs.inputManifest.string().c_str(), mco::FileStream::Read);
					char buf[512] {};
					while(true) {
						n = fh.read(reinterpret_cast<std::uint8_t*>(&buf[0]), sizeof(buf));
						if(n == 0)
							break;
						buffer.append(&buf[0], n);
					}
				}

				currentArgs.manifest = daw::json::from_json<ManifestRoot>(std::move(buffer));
			} catch(daw::json::json_exception& ex) {
				std::fprintf(stderr, "Error when parsing manifest: %s\n", ex.what());
				return 1;
			}

			// Catch an attempt to write a sector-aligned package for an invalid pak version.
			if(currentArgs.manifest.alignment.has_value()) {
				if(*currentArgs.manifest.alignment == eio::pak::Writer::SectorAlignment::Align) {
					if(currentArgs.manifest.version != estructs::PakVersion::Ver5) {
						std::fprintf(stderr, "Cannot write a sector-aligned package if not version 5\n");
						return 1;
					}
				}
			}

			return 0;
		}

		int run() override {
			const auto& jsonManifest = currentArgs.manifest;
			auto fileCount = jsonManifest.files.size();

			std::cout << "Going to write " << fileCount << " files into " << currentArgs.outputFile << '\n';

			if(jsonManifest.alignment.has_value()) {
				if(*jsonManifest.alignment == eio::pak::Writer::SectorAlignment::Align) {
					std::cout << "Writing a sector aligned package\n";
				}
			}

			std::vector<eio::pak::Writer::FlattenedType> files;
			std::vector<std::string> fileTocOrder;

			files.reserve(fileCount);
			fileTocOrder.reserve(fileCount);

			eio::pak::Writer::Manifest eioManifest(files, fileTocOrder);
			eioManifest.version = jsonManifest.version;
			eioManifest.creationUnixTime = jsonManifest.creationTime.value_or(0);

			using enum eio::pak::Writer::SectorAlignment;
			eioManifest.sectorAlignment = jsonManifest.alignment.value_or(DoNotAlign);

			// Create eio pak files from the manifest data.
			for(auto& ent : jsonManifest.files) {
				eio::pak::File file;
				eio::pak::FileData pakData = eio::pak::FileData::newPath(ent.sourcePath);

				file.init(eioManifest.version, jsonManifest.alignment.value_or(DoNotAlign) == Align);

				// Add data
				file.setData(std::move(pakData));
				file.setCreationUnixTime(ent.creationTime.value_or(0));
				files.emplace_back(std::make_pair(ent.path, std::move(file)));
			}

			eioManifest.tocOrder = jsonManifest.tocOrder;

			// After this we no longer need the converted JSON data, so
			// we can just destroy it.
			currentArgs.manifest = {};

			try {
				auto ofs = mco::FileStream::open(currentArgs.outputFile.string().c_str(), mco::FileStream::ReadWrite | mco::FileStream::Create);
				CreateArchiveReportSink reportSink(fileCount);
				eio::pak::Writer writer;

				// Do it.
				writer.writePackage(ofs, reportSink, eioManifest);
			} catch(std::exception& ex) {
				std::cout << "Caught exception: " << ex.what() << "\n";
				return 1;
			}

			return 0;
		}

	   private:
		struct Arguments {
			fs::path inputManifest;
			fs::path outputFile;
			ManifestRoot manifest;

			bool verbose;
		};

		argparse::ArgumentParser parser;
		Arguments currentArgs;
	};

	TOOLLIB_REGISTER_TOOLCOMMAND("eupak_create", CreateCommand);

} // namespace eupak
