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

} // namespace europa::util