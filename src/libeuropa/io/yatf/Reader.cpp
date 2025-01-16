//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <europa/io/yatf/Reader.hpp>
#include <format>
#include <stdexcept>

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

		auto imageSize = util::Size { static_cast<std::uint16_t>(header.width), static_cast<std::uint16_t>(header.height) };

		surface.Resize(imageSize);

		using enum structs::YatfHeader::TextureFormat;
		switch(header.format) {
			case kTextureFormat8Bpp: {
				util::Pixel palette[256] {};
				util::UniqueArray<std::uint8_t> palettizedData(imageSize.Linear());

				stream.read(reinterpret_cast<char*>(&palette[0]), sizeof(palette));
				stream.read(reinterpret_cast<char*>(&palettizedData[0]), imageSize.Linear());

				auto* pDestBuffer = reinterpret_cast<util::Pixel*>(surface.GetBuffer());

				for(std::size_t y = 0; y < imageSize.height; ++y) {
					for(std::size_t x = 0; x < imageSize.width; ++x) {
						auto& pp = palettizedData[y * imageSize.width + x];
						auto& dst = pDestBuffer[y * imageSize.width + x];
						dst = palette[static_cast<std::size_t>(pp)];
					}
				}
			} break;

			case kTextureFormat24Bpp: {
				util::UniqueArray<util::PixelRGB> rgbPixelData(imageSize.Linear());
				stream.read(reinterpret_cast<char*>(&rgbPixelData[0]), imageSize.LinearWithStride<util::PixelRGB>());
				auto* pDestBuffer = reinterpret_cast<util::Pixel*>(surface.GetBuffer());

				for(std::size_t y = 0; y < imageSize.height; ++y) {
					for(std::size_t x = 0; x < imageSize.width; ++x) {
						auto& pp = rgbPixelData[y * imageSize.width + x];
						auto& dst = pDestBuffer[y * imageSize.width + x];
						dst = util::Pixel::FromPixelRGB(pp);
					}
				}
			} break;

			case kTextureFormat32Bpp:
				// We can directly read data
				stream.read(reinterpret_cast<char*>(surface.GetBuffer()), imageSize.LinearWithStride<util::Pixel>());
				break;

			case kTextureFormatUnknown:
			default:
				throw std::runtime_error(std::format("Unknown/unsupported texture format {:02x}!", (std::uint16_t)header.format));
				break;
		}

		return true;
	}

} // namespace europa::io::yatf