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

#include <algorithm>
#include <europa/util/TupleElement.h>

namespace europa::io {

	void PakWriter::Init(structs::PakHeader::Version version) {
		// for now.
		pakHeader.Init(version);
	}

	std::unordered_map<std::string, PakFile>& PakWriter::GetFiles() {
		return archiveFiles;
	}

	// move to a util/ header

	template<class T>
	constexpr T AlignBy(T value, std::size_t alignment) {
		return (-value) & alignment - 1;
	}

	/**
	 * Class functor for flattening a map.
	 */
	template<class Map>
	struct MapFlatten {
		/**
		 * Storage type to store one key -> value pair.
		 */
		using FlattenedType = std::pair<typename Map::key_type, typename Map::mapped_type>;
		using ArrayType = std::vector<FlattenedType>;

		constexpr explicit MapFlatten(Map& mapToFlatten)
			: map(mapToFlatten) {

		}

		ArrayType operator()() const {
			ArrayType arr;
			arr.reserve(map.size());

			for(auto& [ key, value ] : map)
				arr.emplace_back(std::make_pair(key, value));

			return arr;
		}

		private:
		 Map& map;
	};

	// TODO:
	// 	 - Composable operations (WriteTOC, WriteFile, WriteHeader)
	//	 - Add IProgressReportSink reporting

	void PakWriter::Write(std::ostream& os) {

		// This essentially converts our map we use for faster insertion
		// into a flat array we can sort easily.
		//
		// NB: this copies by value, so during this function we use 2x the ram.
		// doesn't seem to be a big problem though.
		auto sortedFiles = MapFlatten{archiveFiles}();

		// Sort the flattened array by file size, the biggest first.
		// Doesn't seem to help (neither does name length)
		std::ranges::sort(sortedFiles, std::greater{}, [](const decltype(MapFlatten{archiveFiles})::FlattenedType& elem) {
			return std::get<1>(elem).GetTOCEntry().size;
		});

		// Leave space for the header
		os.seekp(sizeof(structs::PakHeader), std::ostream::beg);

		// Version 5 paks seem to have an additional bit of reserved data
		// (which is all zeros.)
		if(pakHeader.version == structs::PakHeader::Version::Ver5) {
			os.seekp(6, std::ostream::cur);
		}

		// Write file data
		for(auto& [filename, file] : sortedFiles) {
			//std::cout << "PakWriteFile \"" << filename << "\"\n    Size " << file.GetTOCEntry().size << "\n";

			file.GetTOCEntry().offset = os.tellp();
			os.write(reinterpret_cast<const char*>(file.GetData().data()), file.GetData().size());

			// Flush on file writing
			os.flush();
		}

		pakHeader.tocOffset = os.tellp();

		// Write the TOC
		for(auto& [filename, file] : sortedFiles) {
			file.FillTOCEntry();

			// Write the pstring
			os.put(static_cast<char>(filename.length() + 1));
			for(const auto c : filename)
				os.put(c);
			os.put('\0');

			impl::WriteStreamType(os, file.GetTOCEntry());
		}

		// Fill out the rest of the header.
		pakHeader.fileCount = archiveFiles.size();
		pakHeader.tocSize = static_cast<std::uint32_t>(os.tellp()) - (pakHeader.tocOffset - 1);

		// As the last step, write it.
		os.seekp(0, std::ostream::beg);
		impl::WriteStreamType(os, pakHeader);
	}

} // namespace europa::io