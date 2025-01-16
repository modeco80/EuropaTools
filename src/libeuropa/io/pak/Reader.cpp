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
#include <stdexcept>

#include "../StreamUtils.h"

namespace europa::io::pak {

	Reader::Reader(std::istream& is)
		: stream(is) {
	}

	template <class T>
	void Reader::ReadHeaderAndTOCImpl() {
		auto header_type = impl::ReadStreamType<T>(stream);

		if(!header_type.Valid()) {
			invalid = true;
			return;
		}

		// Read the archive TOC
		stream.seekg(header_type.tocOffset, std::istream::beg);
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
		stream.seekg(0, std::istream::beg);

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

	void Reader::ReadFiles() {
		for(auto& [filename, file] : files)
			ReadFile(filename);
	}

	void Reader::ReadFile(const std::string& file) {
		auto it = std::find_if(files.begin(), files.end(), [&file](Reader::FlatType& fl) { return fl.first == file; });
		if(it == files.end())
			return;

		auto& fileObject = it->second;
		std::vector<std::uint8_t> buffer;

		buffer.resize(fileObject.GetSize());

		// This file was already read in, or has data
		// the user may not want to overwrite.
		if(fileObject.HasData())
			return;

		stream.seekg(fileObject.GetOffset(), std::istream::beg);
		stream.read(reinterpret_cast<char*>(&buffer[0]), buffer.size());

		if(!stream)
			throw std::runtime_error("Stream went bad while trying to read file");

		auto data = FileData::InitAsBuffer(std::move(buffer));
		fileObject.SetData(std::move(data));
	}

	Reader::MapType& Reader::GetFiles() {
		return files;
	}

	const Reader::MapType& Reader::GetFiles() const {
		return files;
	}

} // namespace europa::io::pak