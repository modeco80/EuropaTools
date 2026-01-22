//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstddef>
#include <cstdint>
#include <europa/util/UniqueArray.hpp>

namespace europa::util {

	struct Size {
		u16 width;
		u16 height;

		constexpr std::size_t linear() const {
			return static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
		}

		template <class T>
		constexpr std::size_t linearWithStride() const {
			// We don't cast here since decltype(sizeof(...)) should
			// always be size_t or a size_t bit-compatible type.
			// (The C++ standard pretty much enforces #1 however)
			return linear() * sizeof(T);
		}
	};

	struct [[gnu::packed]] PixelRGB {
		u8 r;
		u8 g;
		u8 b;
	};

	struct [[gnu::packed]] Pixel {
		u8 r;
		u8 g;
		u8 b;
		u8 a;

		static constexpr Pixel fromRGB(const PixelRGB& rgb) {
			return {
				.r = rgb.r,
				.g = rgb.g,
				.b = rgb.b,
				.a = 0xff
			};
		}

		// FIXME: Implement Pixel::fromRGB565 method for 16bpp
	};

	/// A RGBA8888 image surface.
	struct ImageSurface {
		ImageSurface();
		explicit ImageSurface(Size size);

		Size getSize() const;

		u32* getBuffer();

		u32 const* getBuffer() const;

		void resize(Size newSize);

		// FIXME: For now, these APIs will work. It may actually make sense
		// to have a ImageSurface hold a Unique<IPixelBuffer>, that knows about
		// format and can optionally output pixels or provide raw buffer access
		// if desired.
		//
		// basically support other than RGBA8888 out of the box and deal with it nicely

		/// Paint from a 4bpp source.
		/// Assumes this image has been initalized to the proper size already.
		void paintFromSource_4bpp(u8 const* pSrc, Pixel const* pPalette);

		/// Paint from a 8bpp source.
		/// Assumes this image has been initalized to the proper size already.
		void paintFromSource_8bpp(u8 const* pSrc, Pixel const* pPalette);

	   private:
		UniqueArray<u32> imageBuffer;
		Size size;
	};

} // namespace europa::util
