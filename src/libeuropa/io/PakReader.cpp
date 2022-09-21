//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <cstring>
#include <europa/io/PakReader.hpp>
#include <europa/structs/Pak.hpp>

#include "StreamUtils.h"

namespace europa::io {

	PakReader::PakReader(std::istream& is)
		: stream(is) {
	}

	void PakReader::ReadData() {
		header = impl::ReadStreamType<structs::PakHeader>(stream);

		if(!header.Valid()) {
			invalid = true;
			return;
		}

		// Read the archive TOC
		stream.seekg(header.tocOffset, std::istream::beg);
		for(auto i = 0; i < header.fileCount; ++i) {
			// The first part of the TOC entry is a VLE string,
			// which we don't store inside the type (because we can't)
			//
			// Read this in first.
			auto filename = impl::ReadPString(stream);
			files[filename].GetTOCEntry() = impl::ReadStreamType<structs::PakTocEntry>(stream);
		}
	}

	void PakReader::ReadFiles() {
		for(auto& [filename, file] : files)
			ReadFile(filename);
	}

	void PakReader::ReadFile(const std::string& file) {
		auto& fileObject = files[file];

		// This file was already read in, or has data
		// the user may not want to overwrite.
		if(!fileObject.data.empty())
			return;

		auto& toc = fileObject.GetTOCEntry();
		fileObject.data.resize(toc.size);

		stream.seekg(toc.offset, std::istream::beg);
		stream.read(reinterpret_cast<char*>(&fileObject.data[0]), toc.size);
	}

	PakReader::MapType& PakReader::GetFiles() {
		return files;
	}

	const PakReader::MapType& PakReader::GetFiles() const {
		return files;
	}

} // namespace europa::io