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

		// FIXME: merge this code and make it better
		// i.e: let's share de-palettization between the two.
		// (or upgrade ImageSurface to support multiple formats, and allow conversion to a new one?
		//  that might preclude cleaning it up and making util::image namespace however)

		auto v1 = [&]() {
			using enum structs::YatfHeader::TextureFormat;

			switch(header.format) {
				case kTextureFormatV1_8Bpp: {
					util::Pixel palette[256] {};
					util::UniqueArray<std::uint8_t> palettizedData(imageSize.Linear());

					stream.read(reinterpret_cast<char*>(&palette[0]), sizeof(palette));
					stream.read(reinterpret_cast<char*>(&palettizedData[0]), imageSize.Linear());
					surface.PaintFromSource_8bpp(&palettizedData[0], &palette[0]);
				} break;

				case kTextureFormatV1_24Bpp: {
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

				case kTextureFormatV1_32Bpp:
					// We can directly read data
					stream.read(reinterpret_cast<char*>(surface.GetBuffer()), imageSize.LinearWithStride<util::Pixel>());
					break;

				default:
					throw std::runtime_error(std::format("Unknown/unsupported texture format {:02x}!", (std::uint16_t)header.format));
					break;
			}
		};

		auto v2 = [&]() {
			using enum structs::YatfHeader::TextureFormat;

			switch(header.format) {
				case kTextureFormatV2_4Bpp: {
					util::Pixel palette[16] {};
					util::UniqueArray<std::uint8_t> palettizedData(imageSize.Linear() / 2);
					stream.read(reinterpret_cast<char*>(&palette[0]), sizeof(palette));
					stream.read(reinterpret_cast<char*>(&palettizedData[0]), imageSize.Linear() / 2);
					surface.PaintFromSource_4bpp(&palettizedData[0], &palette[0]);
				} break;

				case kTextureFormatV2_8Bpp: {
					util::Pixel palette[256] {};
					util::UniqueArray<std::uint8_t> palettizedData(imageSize.Linear());

					stream.read(reinterpret_cast<char*>(&palette[0]), sizeof(palette));
					stream.read(reinterpret_cast<char*>(&palettizedData[0]), imageSize.Linear());

					surface.PaintFromSource_8bpp(&palettizedData[0], &palette[0]);
				} break;

				case kTextureFormatV2_24Bpp: {
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

				case kTextureFormatV2_32Bpp:
					// We can directly read data
					stream.read(reinterpret_cast<char*>(surface.GetBuffer()), imageSize.LinearWithStride<util::Pixel>());
					break;

				default:
					throw std::runtime_error(std::format("Unknown/unsupported texture format {:02x}!", (std::uint16_t)header.format));
					break;
			}
		};

		switch(header.version) {
			case structs::YatfHeader::Version::Version1:
				v1();
				break;

			case structs::YatfHeader::Version::Version2:
				v2();
				break;

			default:
				throw std::runtime_error(std::format("Unknown/unsupported YATF version {:02x}!", (std::uint16_t)header.version));
				break;
		}

		return true;
	}

} // namespace europa::io::yatf