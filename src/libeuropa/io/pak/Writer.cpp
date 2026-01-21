//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <algorithm>
#include <chrono>
#include <europa/io/pak/Writer.hpp>
#include <europa/structs/Pak.hpp>
#include <europa/util/AlignHelpers.hpp>
#include <europa/util/Overloaded.hpp>
#include <europa/util/UsefulConstants.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "../StreamUtils.h"
#include <mco/io/file_stream.hpp>
#include <mco/io/stream_utils.hpp>

namespace europa::io::pak {

	/// Wrapper to make sure writes are completing fully. This should allow eupak
	/// to gracefully tell you off later.
	void ensureWrite(mco::WritableStream& os, const void* pBuffer, u64 size) {
		if(auto n = os.write(pBuffer, size); n != size) {
			// TODO: tell *what* short-wrote?
			throw std::runtime_error("Short write in pak::Writer");
		}
	}

	void Writer::Write(mco::WritableStream& os, WriterProgressReportSink& sink, const Manifest& manifest) {
		// Sanity check the manifest.
		if(manifest.tocOrder.size() != manifest.files.size()) {
			throw std::runtime_error("Invalid TOC order (doesn't match files)");
		}

		// Depending on the version, do a mix of runtime/compile-time dispatch to the right
		// package format version we have been told to write.
		switch(manifest.version) {
			case structs::PakVersion::Ver3:
				WriteImpl<structs::PakHeader_V3>(os, sink, manifest);
				break;
			case structs::PakVersion::Ver4:
				WriteImpl<structs::PakHeader_V4>(os, sink, manifest);
				break;
			case structs::PakVersion::Ver5:
				WriteImpl<structs::PakHeader_V5>(os, sink, manifest);
				break;
			default:
				throw std::invalid_argument("Invalid version");
		}
	}

	template <class THeader>
	void Writer::WriteImpl(mco::WritableStream& os, WriterProgressReportSink& sink, const Manifest& manifest) {
		// TODO: assert that os.isRandomAccess() == true, since it's the only way
		// we can function currently.
		THeader pakHeader {};

		// Leave space for the header
		os.seek(sizeof(THeader), mco::Stream::Begin);

		if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
			if(manifest.sectorAlignment == SectorAlignment::Align) {
				pakHeader.sectorAlignment = util::kCDSectorSize;
				pakHeader.sectorAlignedFlag = 1;
			} else {
				pakHeader.sectorAlignment = 0;
				pakHeader.sectorAlignedFlag = 0;
			}
		}

		if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
			// Align the first file to start on the next sector boundary.
			if(manifest.sectorAlignment == SectorAlignment::Align)
				os.seek(util::AlignBy(static_cast<std::size_t>(os.tell()), util::kCDSectorSize), mco::Stream::Begin);
		}

		// Write all the file data
		for(auto& [filename, file] : manifest.files) {
			sink.OnEvent({ WriterProgressReportSink::FileEvent::EventCode::FileWriteBegin,
						   filename });

			// Update the offset to where we currently are, since we will be writing the file there.
			file.visitTOCEntry([&](auto& tocEntry) {
				tocEntry.offset = static_cast<std::uint32_t>(os.tell());
			});

			// For sector alignment.
			if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
				if(manifest.sectorAlignment == SectorAlignment::Align) {
					auto& toc = file.getTOCEntry<structs::PakHeader_V5::TocEntry_SectorAligned>();
					toc.startLBA = static_cast<std::uint32_t>((os.tell() / util::kCDSectorSize));
				}
			}

			auto& fileData = file.GetData();

			// Visit the file data sum type and do the right operation
			// For filesystem paths, we open the file and then tee it into the package file
			// effiently saving a lot of memory usage when packing (trading off some IO overhead,
			// but hey.)
			// For buffers, we just write the buffer.

			// clang-format off
			fileData.visit(overloaded {
				[&](const std::filesystem::path& path) {
					auto fs = mco::FileStream::open(path.string().c_str(), mco::FileStream::Read);
					// Tee data into the package file stream
					mco::teeStreams(fs, os, file.getSize());
				},

				[&](const std::vector<std::uint8_t>& buffer) {
					ensureWrite(os, reinterpret_cast<const char*>(buffer.data()), file.getSize());
				} 
			});
			// clang-format on

			if constexpr(THeader::VERSION == structs::PakVersion::Ver5) {
				// Align to the next sector boundary.
				if(manifest.sectorAlignment == SectorAlignment::Align)
					os.seek(util::AlignBy(static_cast<std::size_t>(os.tell()), util::kCDSectorSize), mco::Stream::Current);
			}

			sink.OnEvent({ WriterProgressReportSink::FileEvent::EventCode::FileWriteEnd,
						   filename });
		}

		pakHeader.tocOffset = static_cast<std::uint32_t>(os.tell());

		sink.OnEvent({ WriterProgressReportSink::PakEvent::EventCode::WritingToc });

		// Write the TOC
		for(auto& filename : manifest.tocOrder) {
			auto it = std::find_if(manifest.files.begin(), manifest.files.end(), [&](const auto& el) {
				return el.first == filename;
			});

			if(it == manifest.files.end()) {
				throw std::runtime_error("Invalid TOC order");
			}

			impl::WritePString(os, filename);
			(*it).second.visitTOCEntry([&](auto& tocEntry) {
				ensureWrite(os, &tocEntry, sizeof(tocEntry));
			});
		}


		sink.OnEvent({ WriterProgressReportSink::PakEvent::EventCode::FillInHeader });

		// Fill out the rest of the header.
		pakHeader.fileCount = static_cast<std::uint32_t>(manifest.files.size());
		pakHeader.tocSize = static_cast<std::uint32_t>(os.tell()) - (pakHeader.tocOffset - 1);

		// The TOC was accidentally constructed in a way which always adds a trailer null byte,
		// so replicate that. We do this after filling in the TOC size, because that size
		// is actually still correct.
		os.put(0);

		pakHeader.creationUnixTime = manifest.creationUnixTime;


		sink.OnEvent({ WriterProgressReportSink::PakEvent::EventCode::WritingHeader });

		// As the last step, write the header.
		os.seek(0, mco::Stream::Begin);
		ensureWrite(os, &pakHeader, sizeof(pakHeader));
	}

} // namespace europa::io::pak
