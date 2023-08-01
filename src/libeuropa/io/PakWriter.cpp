//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <algorithm>
#include <europa/io/PakWriter.hpp>
#include <europa/util/TupleElement.hpp>
#include <iostream>

#include "StreamUtils.h"
#include "europa/structs/Pak.hpp"

namespace europa::io {

	void PakWriter::SetVersion(structs::PakVersion version) {
		// for now.
		this->version = version;
	}

	// move to a util/ header

	template<class T>
	constexpr T AlignBy(T value, std::size_t alignment) {
		return (-value) & alignment - 1;
	}

	void PakWriter::Write(std::ostream &os, std::vector<FlattenedType> &&vec, PakProgressReportSink &sink) {
		switch(version) {
			case structs::PakVersion::Ver3:
				WriteImpl<structs::PakHeader_V3>(os, std::move(vec), sink);
				break;
			case structs::PakVersion::Ver4:
				WriteImpl<structs::PakHeader_V3>(os, std::move(vec), sink);
				break;
			case structs::PakVersion::Ver5:
				WriteImpl<structs::PakHeader_V3>(os, std::move(vec), sink);
				break;
		}
	}

	template<class T>
	void PakWriter::WriteImpl(std::ostream& os,  std::vector<FlattenedType>&& vec, PakProgressReportSink& sink, bool sectorAligned) {

		std::vector<FlattenedType> sortedFiles = std::move(vec);

		T pakHeader{};

		// Sort the flattened array by file size, the biggest first.
		// Doesn't seem to help (neither does name length)
		std::ranges::sort(sortedFiles, std::greater{}, [](const FlattenedType& elem) {
			return elem.second.GetSize();
		});

		// Leave space for the header
		os.seekp(sizeof(T), std::ostream::beg);

		// Version 5 paks seem to have an additional bit of reserved data
		// (which is all zeros.)
		if(T::VERSION == structs::PakVersion::Ver5) {
			os.seekp(6, std::ostream::cur);
		}

		//os.seekp( 
		//	AlignBy(os.tellp(), 2048),
		//	std::istream::beg
		//);

		// Write file data
		for(auto& [filename, file] : sortedFiles) {
			sink.OnEvent({
				PakProgressReportSink::FileEvent::Type::FileBeginWrite,
				filename
			});


			file.Visit([&](auto& tocEntry) {
				tocEntry.offset = os.tellp();
			});

			os.write(reinterpret_cast<const char*>(file.GetData().data()), file.GetSize());

			//os.seekp( 
			//	AlignBy(os.tellp(), 2048),
			//	std::istream::beg
			//);

			// Flush on file writing
			os.flush();

			sink.OnEvent({
				PakProgressReportSink::FileEvent::Type::FileEndWrite,
				filename
			});
		}

		pakHeader.tocOffset = os.tellp();

		sink.OnEvent({
			PakProgressReportSink::PakEvent::Type::WritingToc
		});

		// Write the TOC
		for(auto& [filename, file] : sortedFiles) {
			file.FillTOCEntry();

			// Write the pstring
			os.put(static_cast<char>(filename.length() + 1));
			for(const auto c : filename)
				os.put(c);
			os.put('\0');

			file.Visit([&](auto& tocEntry) {
				impl::WriteStreamType(os, tocEntry);
			});

		}


		sink.OnEvent({
			PakProgressReportSink::PakEvent::Type::FillInHeader
		});

		// Fill out the rest of the header.
		pakHeader.fileCount = sortedFiles.size();
		pakHeader.tocSize = static_cast<std::uint32_t>(os.tellp()) - (pakHeader.tocOffset - 1);


		sink.OnEvent({
			PakProgressReportSink::PakEvent::Type::WritingHeader
		});

		// As the last step, write it.
		os.seekp(0, std::ostream::beg);
		impl::WriteStreamType(os, pakHeader);
	}

} // namespace europa::io