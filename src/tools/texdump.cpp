//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <lodepng.h>

#include <europa/io/yatf/Reader.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "europa/structs/Yatf.hpp"
#include "europa/util/ImageSurface.hpp"

namespace eio = europa::io;
namespace eutil = europa::util;
namespace yatf = eio::yatf;
namespace fs = std::filesystem;

int main(int argc, char** argv) {
	// TODO: use argparse, probably!
	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " [path to PS2 Europa .tex file]\n";
		return 1;
	}

	std::ifstream ifs(argv[1], std::ifstream::binary);

	if(!ifs) {
		std::cout << "Invalid file \"" << argv[1] << "\"\n";
		return 1;
	}

	yatf::Reader reader(ifs);

	europa::structs::YatfHeader yatfHeader;
	eutil::ImageSurface surface;

	if(!reader.ReadImage(yatfHeader, surface)) {
		std::cout << "Invalid YATF file \"" << argv[1] << "\"\n";
		return 1;
	}

	auto outPath = fs::path(argv[1]).replace_extension(".png");
	auto size = surface.GetSize();

	if(auto res = lodepng::encode(outPath.string(), reinterpret_cast<std::uint8_t*>(surface.GetBuffer()), size.width, size.height, LCT_RGBA, 8); res == 0) {
		std::cout << "Wrote image to " << outPath << '\n';
		return 0;
	} else {
		std::cout << "Error encoding PNG: " << lodepng_error_text(res) << "\n";
		return 1;
	}

	return 0;
}