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

	/// A RGBA8888 image surface.
	struct ImageSurface {
		ImageSurface();
		explicit ImageSurface(Size size);

		Size GetSize() const;

		std::uint32_t* GetBuffer();

		std::uint32_t const* GetBuffer() const;

		void Resize(Size newSize);

	   private:
		UniqueArray<std::uint32_t> imageBuffer;
		Size size;
	};

} // namespace europa::util