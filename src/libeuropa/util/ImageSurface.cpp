#include <europa/util/ImageSurface.hpp>

namespace europa::util {
	ImageSurface::ImageSurface() {
		size = Size { .width = 0, .height = 0 };
	}

	ImageSurface::ImageSurface(Size size) {
		Resize(size);
	}

	Size ImageSurface::GetSize() const {
		return size;
	}

	void ImageSurface::Resize(Size newSize) {
		imageBuffer.Resize(newSize.Linear());
		size = newSize;
	}

	std::uint32_t* ImageSurface::GetBuffer() {
		return imageBuffer.Data();
	}

	std::uint32_t const* ImageSurface::GetBuffer() const {
		return imageBuffer.Data();
	}

	void ImageSurface::PaintFromSource_4bpp(std::uint8_t const* pSrc, Pixel const* pPalette) {
		auto* pDestBuffer = reinterpret_cast<util::Pixel*>(imageBuffer.Data());

		// can't really get a better loop to work, so i guess this has to do
		for(std::size_t y = 0; y < size.Linear() / 2; ++y) {
			auto& pp = pSrc[y];
			for(std::size_t b = 0; b < 2; ++b) {
				auto col = ((pp & (0x0F << (b * 4))) >> (b * 4));
				(*pDestBuffer++) = pPalette[static_cast<std::size_t>(col)];
			}
		}
	}

	void ImageSurface::PaintFromSource_8bpp(std::uint8_t const* pSrc, Pixel const* pPalette) {
		auto* pDestBuffer = reinterpret_cast<Pixel*>(imageBuffer.Data());

		for(std::size_t y = 0; y < size.height; ++y) {
			for(std::size_t x = 0; x < size.width; ++x) {
				auto& pp = pSrc[y * size.width + x];
				auto& dst = pDestBuffer[y * size.width + x];
				dst = pPalette[static_cast<std::size_t>(pp)];
			}
		}
	}

} // namespace europa::util