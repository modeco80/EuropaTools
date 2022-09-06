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
			header = impl::ReadStreamType<structs::PakHeader>(stream);
		};

		auto ReadTocEntry = [&]() {
			// The first part of the TOC entry is a VLE string,
			// which we don't store inside the type (because we can't)
			//
			// Read this in first.
			auto filename = impl::ReadPString(stream);

			files[filename].GetTOCEntry() = impl::ReadStreamType<structs::PakTocEntry>(stream);
		};

		ReadHeader();

		// Validate the archive header
		if(std::strcmp(header.magic, structs::PakHeader::VALID_MAGIC) != 0) {
			invalid = true;
			return;
		}

		switch(header.version) {
			case structs::PakVersion::Starfighter:
			case structs::PakVersion::Ver2:
				break;

			default:
				invalid = true;
				return;
		}

		stream.seekg(header.tocOffset, std::istream::beg);

		// Read the archive TOC
		for(auto i = 0; i < header.fileCount; ++i)
			ReadTocEntry();

		// Read all file data in
		for(auto& [filename, file] : files) {
			auto& toc = file.GetTOCEntry();
			file.data.resize(toc.size);

			stream.seekg(toc.offset, std::istream::beg);
			stream.read(reinterpret_cast<char*>(&file.data[0]), toc.size);
		}
	}

	const std::unordered_map<std::string, PakFile>& PakReader::GetFiles() const {
		return files;
	}


} // namespace europa::io