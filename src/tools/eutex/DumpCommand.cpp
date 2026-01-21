//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <lodepng.h>

#include <argparse/argparse.hpp>
#include <CommonDefs.hpp>
#include <europa/io/yatf/Reader.hpp>
#include <europa/util/ImageSurface.hpp>
#include <filesystem>
#include <mco/io/file_stream.hpp>
#include <toollib/ToolCommand.hpp>

namespace eutex {

	namespace yatf = eio::yatf;

	struct DumpCommand : tool::IToolCommand {
		DumpCommand()
			: parser("dump", "", argparse::default_arguments::help) {
			// clang-format off
            parser
                .add_description("Dumps a .tex (YATF) texture file to PNG");
                
            parser
                .add_argument("-o", "--output-file")
                .default_value("")
                .metavar("OUTPUT")
                .help("Directory to extract to.");

            parser
                .add_argument("input")
                .help("Input .tex file")
                .metavar("TEX_FILE");
			// clang-format on
		}

		void Init(argparse::ArgumentParser& parentParser) override {
			parentParser.add_subparser(parser);
		}

		bool ShouldRun(argparse::ArgumentParser& parentParser) const override {
			return parentParser.is_subcommand_used("dump");
		}

		int Parse() override {
			inputTexPath = fs::path(parser.get("input"));

			if(parser.is_used("--output-file")) {
				outputPngPath = fs::path(parser.get("--output-file"));
			} else {
				// To maintain workflow compatibility with the older `texdump` utility,
				// if an output file is not specified, we just replace extension
				outputPngPath = fs::path(parser.get("input")).replace_extension("png");
			}

			return 0;
		}

		int Run() override {
			if(!fs::is_regular_file(inputTexPath)) {
				std::cout << "Invalid file " << inputTexPath << "\n";
				return 1;
			}


			auto ifs = mco::FileStream::open(inputTexPath.string().c_str(), mco::FileStream::Read);
			yatf::Reader reader(ifs);

			europa::structs::YatfHeader yatfHeader;
			eutil::ImageSurface surface;

			std::cout << "Opening \"" << inputTexPath << "\"\n";

			if(!reader.readImage(yatfHeader, surface)) {
				std::cout << "Invalid YATF file \"" << inputTexPath << "\"\n";
				return 1;
			}

			auto size = surface.getSize();

			if(auto res = lodepng::encode(outputPngPath.string(), reinterpret_cast<std::uint8_t*>(surface.getBuffer()), size.width, size.height, LCT_RGBA, 8); res == 0) {
				std::cout << "Wrote image to " << outputPngPath << '\n';
				return 0;
			} else {
				std::cout << "Error encoding PNG: " << lodepng_error_text(res) << "\n";
				return 1;
			}

			return 0;
		}

	   private:
		argparse::ArgumentParser parser;

		fs::path inputTexPath;
		fs::path outputPngPath;
	};

	TOOLLIB_REGISTER_TOOLCOMMAND("eutex_dump", DumpCommand);

} // namespace eutex
