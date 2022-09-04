//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <europa/io/PakReader.h>
#include <europa/structs/Pak.h>

#include <cstring>

#include "StreamUtils.h"

namespace europa::io {

	PakReader::PakReader(std::istream& is)
		: stream(is) {
	}

	void PakReader::ReadData() {
		auto ReadHeader = [&]() {
			header = LameRead<structs::PakHeader>(stream);
		};

		auto ReadTocEntry = [&]() {
			// The first part of the TOC entry is a VLE string,
			// which we don't store inside the type (because we can't)
			//
			// Read this in first.
			auto filename = ReadPString(stream);

			// Then read in the rest.
			tocData[filename] = LameRead<structs::PakTocEntry>(stream);
		};

		ReadHeader();

		// Validate the archive magic
		if(std::strcmp(header.magic, structs::PakHeader::VALID_MAGIC) != 0) {
			invalid = true;
			return;
		}

		// std::cout << (int)header.headerSize << " version " << (int)header.version  << '\n';

		stream.seekg(header.tocOffset, std::istream::beg);

		// Read the archive TOC
		for(auto i = 0; i < header.fileCount; ++i)
			ReadTocEntry();

		//		for(auto& [ filename, data ] : tocData) {
		//			std::cout << filename << " offset " << data.offset << " size " << data.size << '\n';
		//		}

		// Read all files in
		for(auto& [filename, data] : tocData) {
			std::vector<std::uint8_t> dataBuffer;

			dataBuffer.resize(data.size);
			stream.seekg(data.offset, std::istream::beg);

			stream.read(reinterpret_cast<char*>(&dataBuffer[0]), data.size);

			File file(std::move(dataBuffer), data);
			files.insert_or_assign(filename, file);
		}
	}

	const std::unordered_map<std::string, PakReader::File>& PakReader::GetFiles() const {
		return files;
	}

	PakReader::File::File(std::vector<std::uint8_t>&& data, structs::PakTocEntry& tocData)
		: data(std::move(data)),
		  tocData(tocData) {
	}

	const std::vector<std::uint8_t>& PakReader::File::GetData() const {
		return data;
	}

	const structs::PakTocEntry& PakReader::File::GetTOCEntry() const {
		return tocData;
	}

} // namespace europa::io