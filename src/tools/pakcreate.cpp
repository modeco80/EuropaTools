//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

// A test utility to regurgitate a pak.

#include <europa/io/PakReader.h>
#include <europa/io/PakWriter.h>

#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

using namespace europa;

int main(int argc, char** argv) {
	std::ofstream ofs(argv[2], std::ofstream::binary);

	if(!ofs) {
		std::cout << "Couldn't open output PAK file\n";
		return 1;
	}

	io::PakWriter writer;

	if(argv[3] != nullptr) {
		if(!strcmp(argv[3], "--jedi")) {
			std::cout << "Writing Jedi Starfighter archive\n";
			writer.Init(structs::PakVersion::Ver2);
		}
	} else {
		std::cout << "Writing Starfighter archive\n";
		writer.Init(structs::PakVersion::Starfighter);
	}

	for(auto& ent : fs::recursive_directory_iterator(argv[1])) {
		if(ent.is_directory())
			continue;

		auto relativePathName = fs::relative(ent.path(), argv[1]).string();

		// Convert to Windows path separator always (that's what the game wants, after all)
		for(auto& c : relativePathName)
			if(c == '/')
				c = '\\';

		std::ifstream ifs(ent.path(), std::ifstream::binary);

		if(!ifs) {
			std::cout << "ERROR: Couldn't open file for archive path \"" << relativePathName << "\"\n";
			return 1;
		}

		io::PakFile file;
		io::PakFile::DataType pakData;

		ifs.seekg(0, std::ifstream::end);
		pakData.resize(ifs.tellg());
		ifs.seekg(0, std::ifstream::beg);

		ifs.read(reinterpret_cast<char*>(&pakData[0]), pakData.size());

		file.SetData(std::move(pakData));
		file.FillTOCEntry();

		file.GetTOCEntry().creationUnixTime = 0;

		//std::cout << "File \"" << relativePathName << "\"\n";
		writer.GetFiles()[relativePathName] = std::move(file);
	}

	writer.Write(ofs);

	std::cout << "Wrote archive to \"" << argv[2] << "\"!\n";
	return 0;
}