//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <europa/io/YatfReader.h>
#include "StreamUtils.h"

#include <vector>

namespace europa::io {

	YatfReader::YatfReader(std::istream& is)
		: stream(is) {
		Init(stream);
	}

	void YatfReader::Init(std::istream& is) {
		// Read the image header.
		header = impl::ReadStreamType<structs::YatfHeader>(stream);

		if(!header.IsValid())
			invalid = true;
	}

	void YatfReader::ReadImage() {
		if(header.flags & structs::YatfHeader::TextureFlag_NoPalette) {
			image.Resize({ static_cast<std::uint16_t>(header.width), static_cast<std::uint16_t>(header.height) });
			stream.read(reinterpret_cast<char*>(image.GetBuffer()), (header.width * header.height) * sizeof(pixel::RgbaColor));
		} else {
			pixel::RgbaColor palette[256];
			std::vector<std::uint8_t> tempBuffer((header.width * header.height));

			// NB: sizeof() does pre-multiplication, so it's 100% ok for us to do this.
			stream.read(reinterpret_cast<char*>(&palette[0]), sizeof(palette));
			stream.read(reinterpret_cast<char*>(&tempBuffer[0]), tempBuffer.size());

			image.Resize({ static_cast<std::uint16_t>(header.width), static_cast<std::uint16_t>(header.height) });

			auto* buffer = image.GetBuffer();
			const auto* data = &tempBuffer[0];

			for(std::size_t i = 0; i < header.width * header.height; ++i)
				*(buffer++) = palette[data[i]];
		}
	}

	pixel::RgbaImage& YatfReader::GetImage() {
		return image;
	}

	const structs::YatfHeader& YatfReader::GetHeader() const {
		return header;
	}



}