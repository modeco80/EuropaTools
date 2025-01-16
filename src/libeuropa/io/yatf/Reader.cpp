//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

// FIXME: Drop libpixel dependency. Instead
// we should just use stbiw directly and provide our own
// simpler/faster utilities for image buffers.

#include <europa/io/yatf/Reader.hpp>
#include <stdexcept>
#include <vector>

#include "../StreamUtils.h"
#include "europa/structs/Yatf.hpp"
#include "europa/util/ImageSurface.hpp"

namespace europa::io::yatf {

	Reader::Reader(std::istream& is)
		: stream(is) {
	}

	bool Reader::ReadImage(structs::YatfHeader& header, util::ImageSurface& surface) {
		header = impl::ReadStreamType<structs::YatfHeader>(stream);

		if(!header.IsValid()) {
			return false;
		}

		surface.Resize({ static_cast<std::uint16_t>(header.width), static_cast<std::uint16_t>(header.height) });

		if(header.flags & structs::YatfHeader::TextureFlag_NoPalette) {
			stream.read(reinterpret_cast<char*>(surface.GetBuffer()), (header.width * header.height) * 4);
		} else {
			/*
			pixel::RgbaColor palette[256];
			std::vector<std::uint8_t> tempBuffer((header.width * header.height));

			// NB: sizeof() does pre-multiplication, so it's 100% ok for us to do this.
			stream.read(reinterpret_cast<char*>(&palette[0]), sizeof(palette));
			stream.read(reinterpret_cast<char*>(&tempBuffer[0]), tempBuffer.size());

			auto* buffer = image.GetBuffer();
			const auto* data = &tempBuffer[0];

			for(std::size_t i = 0; i < header.width * header.height; ++i)
				*(buffer++) = palette[data[i]];
			*/

			throw std::runtime_error("FIXME: Port this path properly :(");
		}

		return true;
	}

} // namespace europa::io::yatf