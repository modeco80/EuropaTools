//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <cstdint>
#include <cstring>
#include <europa/io/PakReader.hpp>
#include <europa/structs/Pak.hpp>
#include <stdexcept>

#include "europa/io/PakFile.hpp"
#include "StreamUtils.h"

namespace europa::io {

	PakReader::PakReader(std::istream& is)
		: stream(is) {
	}

	template <class T>
	void PakReader::ReadData_Impl() {
		auto header_type = impl::ReadStreamType<T>(stream);

		if(!header_type.Valid()) {
			invalid = true;
			return;
		}

		//		bool isStreams { false };

		//		if(header_type.tocOffset > 0x17000000)
		//			isStreams = true;

		// Read the archive TOC
		stream.seekg(header_type.tocOffset, std::istream::beg);
		for(auto i = 0; i < header_type.fileCount; ++i) {
			// The first part of the TOC entry is always a VLE string,
			// which we don't store inside the type (because we can't)
			//
			// Read this in first.
			auto filename = impl::ReadPString(stream);
			files[filename].InitWithExistingTocEntry(impl::ReadStreamType<typename T::TocEntry>(stream));

			// Don't think this is needed
			// if(isStreams)
			//	files[filename].Visit([&](auto& tocEntry) {
			//		tocEntry.creationUnixTime = impl::ReadStreamType<structs::u32>(stream);
			//	});
		}

		header = header_type;
	}

	void PakReader::ReadData() {
		auto commonHeader = impl::ReadStreamType<structs::PakHeader_Common>(stream);
		stream.seekg(0, std::istream::beg);

		switch(commonHeader.version) {
			case structs::PakVersion::Ver3:
				ReadData_Impl<structs::PakHeader_V3>();
				break;
			case structs::PakVersion::Ver4:
				ReadData_Impl<structs::PakHeader_V4>();
				break;
			case structs::PakVersion::Ver5:
				ReadData_Impl<structs::PakHeader_V5>();
				break;
			default:
				return;
		}
	}

	void PakReader::ReadFiles() {
		for(auto& [filename, file] : files)
			ReadFile(filename);
	}

	void PakReader::ReadFile(const std::string& file) {
		auto& fileObject = files[file];
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

		auto data = PakFileData::InitAsBuffer(std::move(buffer));
		fileObject.SetData(std::move(data));
	}

	PakReader::MapType& PakReader::GetFiles() {
		return files;
	}

	const PakReader::MapType& PakReader::GetFiles() const {
		return files;
	}

} // namespace europa::io