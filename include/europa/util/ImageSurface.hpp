//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <europa/util/UniqueArray.hpp>
#include <europa/util/DllExport.hpp>

namespace europa::util {

	struct Size {
		std::uint16_t width;
		std::uint16_t height;

		constexpr std::size_t Linear() const {
			return static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
		}

		template <class T>
		constexpr std::size_t LinearWithStride() const {
			// We don't cast here since decltype(sizeof(...)) should
			// always be size_t or a size_t bit-compatible type.
			// (The C++ standard pretty much enforces #1 however)
			return Linear() * sizeof(T);
		}
	};

// Avoid dependency on <pshpack*> header
// while still allowing Pixel to be properly packed
#ifdef _MSC_VER
	#pragma pack(push, 1)
#endif

	struct [[gnu::packed]] PixelRGB {
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
	};

	struct [[gnu::packed]] Pixel {
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;

		static constexpr Pixel FromPixelRGB(const PixelRGB& rgb) {
			return {
				.r = rgb.r,
				.g = rgb.g,
				.b = rgb.b,
				.a = 0xff
			};
		}

		// FIXME: Implement Pixel::FromRgb565 method for 16bpp
	};

#ifdef _MSC_VER
	#pragma pack(pop)
#endif

	/// A RGBA8888 image surface.
	struct LIBEUROPA_EXPORT ImageSurface {
		ImageSurface();
		explicit ImageSurface(Size size);

		Size GetSize() const;

		std::uint32_t* GetBuffer();

		std::uint32_t const* GetBuffer() const;

		void Resize(Size newSize);

		// FIXME: For now, these APIs will work. It may actually make sense 
		// to have a ImageSurface hold a Unique<IPixelBuffer>, that knows about
		// format and can optionally output pixels or provide raw buffer access
		// if desired.
		//
		// basically support other than RGBA8888 out of the box and deal with it nicely

		/// Paint from a 4bpp source.
		/// Assumes this image has been initalized to the proper size already.
		void PaintFromSource_4bpp(std::uint8_t const* pSrc, Pixel const* pPalette);

		/// Paint from a 8bpp source.
		/// Assumes this image has been initalized to the proper size already.
		void PaintFromSource_8bpp(std::uint8_t const* pSrc, Pixel const* pPalette);

	   private:
		UniqueArray<std::uint32_t> imageBuffer;
		Size size;
	};

} // namespace europa::util