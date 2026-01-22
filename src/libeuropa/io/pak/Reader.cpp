//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <europa/io/pak/File.hpp>
#include <europa/io/pak/Reader.hpp>
#include <europa/structs/Pak.hpp>
#include <stdexcept>

#include "../StreamUtils.h"

namespace europa::io::pak {

	// FIXME: Expose this exception..
	struct ReaderInvalidPath : public std::exception {
		const char* what() const noexcept override {
			return "no such path in package file";
		}
	};

	Reader::OpenedFile::OpenedFile(mco::Stream& source, File& file)
		: source(source), file(file) {
		virtualPosition = 0;
	}

	bool Reader::OpenedFile::isRandomAccess() const noexcept {
		return true;
	}

	u64 Reader::OpenedFile::read(void* buffer, u64 length) {
		const auto fileSize = file.getSize();

		// just to be sure!
		seek(virtualPosition, mco::Stream::Begin);

		// EOF handling
		if(virtualPosition >= fileSize) {
			return 0;
		}

		usize readCount = length;
		if(virtualPosition + length > fileSize)
			readCount = fileSize - virtualPosition;

		auto nRead = source.read(buffer, readCount);
		virtualPosition += nRead;
		return nRead;
	}

	i64 Reader::OpenedFile::tell() {
		return virtualPosition;
	}

	i64 Reader::OpenedFile::seek(i64 offset, Whence whence) {
		u64 target = 0;
		switch(whence) {
			case mco::Stream::Begin:
				target = offset;
				break;
			case mco::Stream::Current:
				target = virtualPosition + offset;
				break;
			case mco::Stream::End:
				if(offset > 0)
					return -1;
				target = file.getSize() + offset;
				break;
		}

		if(target > file.getSize()) {
			// too far
			return -1;
		}

		source.seek(file.getOffset() + target, mco::Stream::Begin);
		virtualPosition = target;
		return virtualPosition;
	}

	u64 Reader::OpenedFile::getSize() {
		return file.getSize();
	}

	/// Reader

	Reader::Reader(mco::Stream& is)
		: stream(is) {
	}

	template <class T>
	void Reader::initImpl() {
		// Read the header. If this fails, or it's invalid, give up.
		auto pakHeader = impl::ReadStreamType<T>(stream);
		if(!pakHeader.valid()) {
			throw std::runtime_error("Invalid Package file.");
		}

		// Read the archive TOC.
		stream.seek(pakHeader.tocOffset, mco::Stream::Begin);
		for(u32 i = 0; i < pakHeader.fileCount; ++i) {
			// The first part of the TOC entry is always a VLE string,
			// which we don't store inside the type (because we can't).
			auto filename = impl::ReadPString(stream);

			auto file = File {};
			if constexpr(std::is_same_v<T, structs::PakHeader_V5>) {
				// Version 5 supports sector aligned packages, which have an additional field in them
				// (the start LBA of the file), so we need to handle that here.
				if(pakHeader.sectorAlignedFlag) {
					file.init(impl::ReadStreamType<typename T::TocEntry_SectorAligned>(stream));
				} else {
					file.init(impl::ReadStreamType<typename T::TocEntry>(stream));
				}
			} else {
				file.init(impl::ReadStreamType<typename T::TocEntry>(stream));
			}

			files.emplace_back(filename, std::move(file));
		}

		header = pakHeader;
	}

	void Reader::init() {
		auto commonHeader = impl::ReadStreamType<structs::PakHeader_Common>(stream);
		stream.seek(0, mco::Stream::Begin);

		switch(commonHeader.version) {
			case structs::PakVersion::Ver3:
				initImpl<structs::PakHeader_V3>();
				break;
			case structs::PakVersion::Ver4:
				initImpl<structs::PakHeader_V4>();
				break;
			case structs::PakVersion::Ver5:
				initImpl<structs::PakHeader_V5>();
				break;
			default:
				return;
		}
	}

	Reader::OpenedFile Reader::open(const std::string& path) {
		auto it = std::find_if(files.begin(), files.end(), [&path](Reader::FlatType& fl) { return fl.first == path; });
		if(it == files.end())
			throw ReaderInvalidPath();
		return Reader::OpenedFile(stream, it->second);
	}

	const Reader::MapType& Reader::getPackageFiles() const {
		return files;
	}

} // namespace europa::io::pak
