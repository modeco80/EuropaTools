#include <europa/util/ImageSurface.hpp>

namespace europa::util {
	ImageSurface::ImageSurface() {
		size = Size { .width = 0, .height = 0 };
	}

	ImageSurface::ImageSurface(Size size) {
		resize(size);
	}

	Size ImageSurface::getSize() const {
		return size;
	}

	void ImageSurface::resize(Size newSize) {
		imageBuffer.resize(newSize.linear());
		size = newSize;
	}

	std::uint32_t* ImageSurface::getBuffer() {
		return imageBuffer.data();
	}

	std::uint32_t const* ImageSurface::getBuffer() const {
		return imageBuffer.data();
	}

	void ImageSurface::paintFromSource_4bpp(std::uint8_t const* pSrc, Pixel const* pPalette) {
		auto* pDestBuffer = reinterpret_cast<util::Pixel*>(imageBuffer.data());

		// can't really get a better loop to work, so i guess this has to do
		for(std::size_t y = 0; y < size.linear() / 2; ++y) {
			auto& pp = pSrc[y];
			for(std::size_t b = 0; b < 2; ++b) {
				auto col = ((pp & (0x0F << (b * 4))) >> (b * 4));
				(*pDestBuffer++) = pPalette[static_cast<std::size_t>(col)];
			}
		}
	}

	void ImageSurface::paintFromSource_8bpp(std::uint8_t const* pSrc, Pixel const* pPalette) {
		auto* pDestBuffer = reinterpret_cast<Pixel*>(imageBuffer.data());

		for(std::size_t y = 0; y < size.height; ++y) {
			for(std::size_t x = 0; x < size.width; ++x) {
				auto& pp = pSrc[y * size.width + x];
				auto& dst = pDestBuffer[y * size.width + x];
				dst = pPalette[static_cast<std::size_t>(pp)];
			}
		}
	}

} // namespace europa::util
