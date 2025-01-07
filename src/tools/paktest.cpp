//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

// A test utility to regurgitate a pak.

#include <europa/io/PakReader.hpp>
#include <europa/io/PakWriter.hpp>
#include <fstream>
#include <iostream>

using namespace europa;

int main(int argc, char** argv) {
	std::ifstream ifs(argv[1], std::ifstream::binary);
	std::ofstream ofs(argv[2], std::ofstream::binary);

	if(!ifs) {
		std::cout << "Couldn't open input PAK file\n";
		return 1;
	}

	io::PakWriter writer;

	if(argv[3] != nullptr) {
		if(!strcmp(argv[3], "--jedi")) {
			std::cout << "Writing Jedi Starfighter archive\n";
			writer.Init(structs::PakHeader::Version::Ver5);
		}
	} else {
		std::cout << "Writing Starfighter archive\n";
		writer.Init(structs::PakHeader::Version::Ver4);
	}

	// Read pak data and vomit it into the writer.
	// This will temporarily consume 2x the memory (so about 240mb for the biggest paks I've seen),
	// but the writer will contain the first copy,
	// until it's cleared.
	{
		io::PakReader reader(ifs);
		reader.ReadData();

		if(reader.Invalid()) {
			std::cout << "Invalid pak file, exiting\n";
			return 1;
		}

		for(auto& [filename, file] : reader.GetFiles()) {
			std::cout << "Reading \"" << filename << "\" into memory\n";
			reader.ReadFile(filename);
			writer.GetFiles()[filename] = file;
		}
	}

	writer.Write(ofs);

	std::cout << "Wrote regurgitated archive to \"" << argv[2] << "\"!\n";
	return 0;
}