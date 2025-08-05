//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <boost/json/serialize_options.hpp>
#include <boost/json/serializer.hpp>
#include <CommonDefs.hpp>
#include <EupakConfig.hpp>
#include <europa/base/VirtualFileSystem.hpp>
#include <europa/io/pak/Reader.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <system_error>
#include <toollib/ToolCommand.hpp>

#include "europa/io/pak/Writer.hpp"
#include "europa/structs/Pak.hpp"
#include "ManifestJSON.hpp"

namespace eupak {

	void TeeOutPackageFileData(eio::pak::Reader::OpenedPackageFile& is, ebase::VfsFileHandle& os) {
		std::uint8_t buffer[1024] {};
		int rc = 0;
		while(true) {
			rc = is.Read(&buffer[0], sizeof(buffer));
			if(rc == 0) {
				// end of stream
				break;
			}
			os->Write(&buffer[0], rc);
		}
	}

	std::string BeautifyPath(const std::string& path) {
		auto nameCopy = path;

#ifndef _WIN32
		// Replace path seperators with the POSIX ones.
		std::replace(nameCopy.begin(), nameCopy.end(), '\\', '/');
#endif
		return nameCopy;
	}

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
			.required()
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

		void createManifest(const estructs::PakHeaderVariant& header, const eio::pak::Reader::MapType& files) {
			auto outpath = (currentArgs.outputDirectory / "manifest.json");

			ManifestRoot root;

			// set this up
			std::visit([&](auto& pakHdr) {
				root.version = pakHdr.version;
				root.creationTime = pakHdr.creationUnixTime;
			},
					   header);

			// if this is pakv5, then set the alignment appropiately
			// (the default is fine for older versions which don't have alignment)
			if(auto* h = std::get_if<estructs::PakHeader_V5>(&header)) {
				if(h->sectorAlignedFlag == 1) {
					root.alignment = eio::pak::Writer::SectorAlignment::Align;
				} else {
					root.alignment = eio::pak::Writer::SectorAlignment::DoNotAlign;
				}
			}

			eio::pak::Reader::MapType fileOrderedClone(files);

			// Sort our clone of the file table to the order which files were actually written.
			// The files "map" is in TOC order (which can differ to the actual file placement), so we do not need to do anything with that.
			std::sort(fileOrderedClone.begin(), fileOrderedClone.end(), [](const auto& f1, const auto& f2) {
				return f1.second.GetOffset() < f2.second.GetOffset();
			});

			for(auto& [filename, file] : files) {
				//printf("%s toc offset : %08x\n", filename.c_str(), file.GetOffset());
				root.tocOrder.push_back(filename);
			}

			for(auto& [filename, file] : fileOrderedClone) {
				//printf("%s file offset : %08x\n", filename.c_str(), file.GetOffset());
				auto outpath = (currentArgs.outputDirectory / "files" / BeautifyPath(filename));
				root.files.push_back(ManifestFile {
				.path = filename,
				.sourcePath = outpath.string(),
				.creationTime = file.GetCreationUnixTime() });
			}

			// Serialize the JSON to the manifest file
			// This way doesn't use a giant string so I think it's better,
			// if a bit verbose.
			auto fh = ebase::HostFileSystem().Open(outpath.string(), ebase::VirtualFileSystem::Read | ebase::VirtualFileSystem::Write | ebase::VirtualFileSystem::Create);
			fh->Truncate(0);

			using namespace daw::json::options;
			auto dump = daw::json::to_json(root, output_flags<SerializationFormat::Pretty, IndentationType::Tab>);
			fh->Write(reinterpret_cast<const std::uint8_t*>(dump.data()), dump.size());
		}

		int Run() override {
			std::cout << "Input PAK/PMDL: " << currentArgs.inputPath << '\n';
			std::cout << "Output Directory: " << currentArgs.outputDirectory << '\n';

			auto& hostFs = ebase::HostFileSystem();
			auto fh = hostFs.Open(currentArgs.inputPath.string());
			eio::pak::Reader reader(std::move(fh));

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

			if(!fs::exists(currentArgs.outputDirectory))
				fs::create_directories(currentArgs.outputDirectory);

			// Create the manifest file.
			createManifest(reader.GetHeader(), reader.GetFiles());

			for(auto& [filename, file] : reader.GetFiles()) {
				auto nameCopy = BeautifyPath(filename);
				progress.set_option(indicators::option::PostfixText { filename });

				auto outpath = (currentArgs.outputDirectory / "files" / nameCopy);

				if(!fs::exists(outpath.parent_path()))
					fs::create_directories(outpath.parent_path());

				if(currentArgs.verbose) {
					std::cerr << "Extracting file \"" << filename << "\"...\n";
				}

				try {
					// Tee out the package file data to the host file.
					auto packageFile = reader.Open(filename);
					auto outputFile = hostFs.Open(outpath.string(), ebase::VirtualFileSystem::Read | ebase::VirtualFileSystem::Write | ebase::VirtualFileSystem::Create);
					outputFile->Truncate(file.GetSize());
					TeeOutPackageFileData(packageFile, outputFile);
					outputFile.Close();
				} catch(std::system_error& err) {
					std::cerr << "Error when writing output file: " << err.what() << "\n";
					return 1;
				}

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