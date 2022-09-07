//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

// A test utility to regurgitate a pak.

#include <europa/io/PakReader.h>
#include <europa/io/PakWriter.h>

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
	std::ifstream ifs(argv[1], std::ifstream::binary);
	std::ofstream ofs("new_archive.pak", std::ofstream::binary);

	europa::io::PakWriter writer;

	writer.Init(europa::structs::PakVersion::Ver2);

	// Read pak data and vomit it into the writer.
	// This will temporarily consume 2x the memory (so about 240mb for the biggest paks I've seen),
	// but the writer will contain the first copy,
	// until it's cleared.
	{
		europa::io::PakReader reader(ifs);
		reader.ReadData();

		for(auto& [filename, file] : reader.GetFiles()) {
			writer.AddFile(filename, file);
		}
	}

	writer.Write(ofs);

	std::cout << "Wrote regurgitated archive to new.pak!\n";
	return 0;
}