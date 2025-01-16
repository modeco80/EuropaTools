//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <algorithm>
#include <chrono>
#include <europa/io/pak/Writer.hpp>
#include <europa/structs/Pak.hpp>
#include <europa/util/AlignHelpers.hpp>
#include <europa/util/Overloaded.hpp>
#include <europa/util/TupleElement.hpp>
#include <europa/util/UsefulConstants.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "../StreamUtils.h"

namespace europa::io::pak {

	void Writer::SetVersion(structs::PakVersion version) {
		// for now.
		this->version = version;
	}

	void Writer::Write(std::ostream& os, std::vector<FlattenedType>&& vec, WriterProgressReportSink& sink, SectorAlignment sectorAlignment) {
		// Depending on the version, do a mix of runtime/compile-time dispatch to the right
		// package format version we have been told to write.
		switch(version) {
			case structs::PakVersion::Ver3:
				WriteImpl<structs::PakHeader_V3>(os, std::move(vec), sink, sectorAlignment);
				break;
			case structs::PakVersion::Ver4:
				WriteImpl<structs::PakHeader_V4>(os, std::move(vec), sink, sectorAlignment);
				break;
			case structs::PakVersion::Ver5:
				WriteImpl<structs::PakHeader_V5>(os, std::move(vec), sink, sectorAlignment);
				break;
			default:
				throw std::invalid_argument("Invalid version");
		}
	}

	template <class THeader>
	void Writer::WriteImpl(std::ostream& os, std::vector<FlattenedType>&& vec, WriterProgressReportSink& sink, SectorAlignment sectorAlignment) {
		std::vector<FlattenedType> sortedFiles = std::move(vec);

		THeader pakHeader {};

		// FIXME: We should not be modifying the user provided order here
		// Instead, a client can choose to sort however they want
		// (Either that or we can explicitly disable this behaviour with a new argument, idk)

		// Sort the flattened array.
		std::ranges::sort(sortedFiles, std::greater {}, [](const FlattenedType& elem) {
			return elem.second.GetSize();
		});

		// Leave space for the header
		os.seekp(sizeof(THeader), std::ostream::beg);

		if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
			if(sectorAlignment == SectorAlignment::Align) {
				pakHeader.sectorAlignment = util::kCDSectorSize;
				pakHeader.sectorAlignedFlag = 1;
			} else {
				pakHeader.sectorAlignment = 0;
				pakHeader.sectorAlignedFlag = 0;
			}
		}

		if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
			// Align the first file to start on the next sector boundary.
			if(sectorAlignment == SectorAlignment::Align)
				os.seekp(util::AlignBy(static_cast<std::size_t>(os.tellp()), util::kCDSectorSize), std::istream::beg);
		}

		// Write all the file data
		for(auto& [filename, file] : sortedFiles) {
			sink.OnEvent({ WriterProgressReportSink::FileEvent::EventCode::FileWriteBegin,
						   filename });

			// Update the offset to where we currently are, since we will be writing the file there
			file.VisitTocEntry([&](auto& tocEntry) {
				tocEntry.offset = os.tellp();
			});

			// For sector alignment.
			if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
				if(sectorAlignment == SectorAlignment::Align) {
					auto& toc = file.GetTOCEntry<structs::PakHeader_V5::TocEntry_SectorAligned>();
					toc.startLBA = (os.tellp() / util::kCDSectorSize);
				}
			}

			auto& fileData = file.GetData();

			// Visit the file data sum type and do the right operation
			// For filesystem paths, we open the file and then tee it into the package file
			// effiently saving a lot of memory usage when packing (trading off some IO overhead,
			// but hey.)
			// For buffers, we just write the buffer.

			// clang-format off
			fileData.Visit(overloaded {
				[&](const std::filesystem::path& path) {
					auto fs = std::ifstream(path.string(), std::ifstream::binary);
					if(!fs)
						throw std::runtime_error("couldnt open input file? HOW");

					// Tee data into the package file stream
					impl::TeeInOut(fs, os);
				},

				[&](const std::vector<std::uint8_t>& buffer) {
					os.write(reinterpret_cast<const char*>(buffer.data()), file.GetSize());
				} 
			});
			// clang-format on

			if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
				// Align to the next sector boundary.
				if(sectorAlignment == SectorAlignment::Align)
					os.seekp(util::AlignBy(static_cast<std::size_t>(os.tellp()), util::kCDSectorSize), std::istream::beg);
			}

			sink.OnEvent({ WriterProgressReportSink::FileEvent::EventCode::FileWriteEnd,
						   filename });
		}

		pakHeader.tocOffset = os.tellp();

		sink.OnEvent({ WriterProgressReportSink::PakEvent::EventCode::WritingToc });

#if 0
		// Sort for toc stuff? idk
		std::ranges::sort(sortedFiles, std::less {}, [](const FlattenedType& elem) {
			return elem.second.GetCreationUnixTime();
		});
#endif

		// Write the TOC
		for(auto& [filename, file] : sortedFiles) {
			// Write the filename Pascal string.
			impl::WritePString(os, filename);

			file.VisitTocEntry([&](auto& tocEntry) {
				impl::WriteStreamType(os, tocEntry);
			});
		}

		sink.OnEvent({ WriterProgressReportSink::PakEvent::EventCode::FillInHeader });

		// Fill out the rest of the header.
		pakHeader.fileCount = sortedFiles.size();
		pakHeader.tocSize = static_cast<std::uint32_t>(os.tellp()) - (pakHeader.tocOffset - 1);

		// Timestamp.
		auto now = std::chrono::system_clock::now();
		pakHeader.creationUnixTime = static_cast<std::uint32_t>(std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count());

		sink.OnEvent({ WriterProgressReportSink::PakEvent::EventCode::WritingHeader });

		// As the last step, write it.
		os.seekp(0, std::ostream::beg);
		impl::WriteStreamType(os, pakHeader);
	}

} // namespace europa::io::pak