//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <europa/io/PakWriter.h>

#include <iostream>

#include "StreamUtils.h"

namespace europa::io {

	void PakWriter::Init(structs::PakVersion version) {
		// for now.
		pakHeader.Init(version);
	}

	std::unordered_map<std::string, PakFile>& PakWriter::GetFiles() {
		return archiveFiles;
	}

	void PakWriter::Write(std::ostream& os) {
		// Set up the header a bit more...
		pakHeader.fileCount = archiveFiles.size();

		// Leave space for the header
		os.seekp(sizeof(structs::PakHeader), std::ostream::beg);

		// Seek forwards for version 2 PAKs, as the only
		// difference seems to be
		if(pakHeader.version == structs::PakVersion::Ver2) {
			os.seekp(6, std::ostream::cur);
		}

		// Write file data
		for(auto& [filename, file] : archiveFiles) {
			file.GetTOCEntry().offset = os.tellp();
			os.write(reinterpret_cast<const char*>(file.GetData().data()), file.GetData().size());
		}

		pakHeader.tocOffset = os.tellp();

		// Write the TOC
		for(auto& [filename, file] : archiveFiles) {
			file.FillTOCEntry();

			// Write the pstring
			os.put(static_cast<char>(filename.length() + 1));
			for(const auto c : filename)
				os.put(c);
			os.put('\0');

			impl::WriteStreamType(os, file.GetTOCEntry());
		}

		os.seekp(0, std::ostream::beg);
		impl::WriteStreamType(os, pakHeader);
	}

} // namespace europa::io