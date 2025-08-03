//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <europa/io/pak/File.hpp>
#include <europa/io/pak/Reader.hpp>
#include <europa/structs/Pak.hpp>
#include <exception>

#include "../StreamUtils.h"
#include "europa/base/VirtualFileSystem.hpp"

namespace europa::io::pak {

	struct ReaderInvalidPath : public std::exception {
		const char* what() const noexcept override {
			return "no such path in package file";
		}
	};

	/// Reader::OpenedPackageFile

	Reader::OpenedPackageFile::OpenedPackageFile(base::VfsFileHandle&& fh)
		: fh(std::move(fh)) {
	}

	void Reader::OpenedPackageFile::Init(File& data) {
		fileOffset = data.GetOffset();
		fileSize = data.GetSize();

		fh->Seek(fileOffset);
	}

	// TODO: This logic would probably be better in a VFS adapter. This would also mean making it possible
	// to make some of the streamutils code common (since really it is probably better there) and maybe
	// bits of it public (teeing a stream is actually relatively useful)?

	std::uint64_t Reader::OpenedPackageFile::Read(std::uint8_t* pBuffer, std::size_t bufferLength) {
		// EOF
		if(virtualPos >= fileSize) {
			return 0;
		}

		std::size_t readCount = bufferLength;
		if(virtualPos + bufferLength > fileSize)
			readCount = fileSize - virtualPos;

		auto nRead = fh->Read(pBuffer, readCount);
		virtualPos += nRead;
		return nRead;
	}

	std::uint64_t Reader::OpenedPackageFile::Tell() {
		return virtualPos;
	}

	std::uint64_t Reader::OpenedPackageFile::Seek(std::int64_t offset, base::VfsFile::SeekDirection seekDir) {
		std::uint64_t target = 0;
		switch(seekDir) {
			case base::VfsFile::RelativeBegin:
				target = offset;
				break;
			case base::VfsFile::RelativeCurrent:
				target = virtualPos + offset;
				break;
			case base::VfsFile::RelativeEnd:
				target = fileSize + offset;
				break;
		}

		if(target > fileSize) {
			// too far
			return -1;
		}

		fh->Seek(fileOffset + target, base::VfsFile::RelativeBegin);
		virtualPos = target;
		return virtualPos;
	}

	/// Reader

	Reader::Reader(base::VfsFileHandle&& is)
		: stream(std::move(is)) {
	}

	template <class T>
	void Reader::ReadHeaderAndTOCImpl() {
		auto header_type = impl::ReadStreamType<T>(stream);

		if(!header_type.Valid()) {
			invalid = true;
			return;
		}

		// Read the archive TOC
		stream->Seek(header_type.tocOffset, base::VfsFile::RelativeBegin);
		for(std::uint32_t i = 0; i < header_type.fileCount; ++i) {
			// The first part of the TOC entry is always a VLE string,
			// which we don't store inside the type (because we can't)
			//
			// Read this in first.
			auto filename = impl::ReadPString(stream);
			auto file = File {};
			if constexpr(std::is_same_v<T, structs::PakHeader_V5>) {
				// Version 5 supports sector aligned packages, which have an additional field in them
				// (the start LBA of the file), so we need to handle that here.
				if(header_type.sectorAlignedFlag) {
					file.InitWithExistingTocEntry(impl::ReadStreamType<typename T::TocEntry_SectorAligned>(stream));
				} else {
					file.InitWithExistingTocEntry(impl::ReadStreamType<typename T::TocEntry>(stream));
				}
			} else {
				file.InitWithExistingTocEntry(impl::ReadStreamType<typename T::TocEntry>(stream));
			}

			files.emplace_back(filename, std::move(file));
		}

		header = header_type;
	}

	void Reader::ReadHeaderAndTOC() {
		auto commonHeader = impl::ReadStreamType<structs::PakHeader_Common>(stream);
		stream->Seek(0, base::VfsFile::RelativeBegin);

		switch(commonHeader.version) {
			case structs::PakVersion::Ver3:
				ReadHeaderAndTOCImpl<structs::PakHeader_V3>();
				break;
			case structs::PakVersion::Ver4:
				ReadHeaderAndTOCImpl<structs::PakHeader_V4>();
				break;
			case structs::PakVersion::Ver5:
				ReadHeaderAndTOCImpl<structs::PakHeader_V5>();
				break;
			default:
				return;
		}
	}

	Reader::OpenedPackageFile Reader::Open(const std::string& file) {
		auto it = std::find_if(files.begin(), files.end(), [&file](Reader::FlatType& fl) { return fl.first == file; });
		if(it == files.end())
			throw ReaderInvalidPath();

		auto& fileObject = it->second;

		// Clone a new file handle.
		auto fh = stream.Clone();

		auto opened = Reader::OpenedPackageFile(std::move(fh));
		opened.Init(fileObject);
		return opened;
	}

	Reader::MapType& Reader::GetFiles() {
		return files;
	}

	const Reader::MapType& Reader::GetFiles() const {
		return files;
	}

} // namespace europa::io::pak