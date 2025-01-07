//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <pixel/ImageWriter.h>

#include <europa/io/YatfReader.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
int main(int argc, char** argv) {
	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " [path to Europa PAK file]";
		return 1;
	}

	std::ifstream ifs(argv[1], std::ifstream::binary);

	if(!ifs) {
		std::cout << "Invalid file \"" << argv[1] << "\"\n";
		return 1;
	}

	europa::io::YatfReader reader(ifs);

	if(reader.Invalid()) {
		std::cout << "Invalid YATF file \"" << argv[1] << "\"\n";
		return 1;
	}

	reader.ReadImage();

	pixel::ImageWriter writer {};

	auto outPath = fs::path(argv[1]).replace_extension(".png");

	writer.SetImage(reader.GetImage());

	writer.WritePng(outPath);

	std::cout << "Wrote image to " << outPath << '\n';

	return 0;
}