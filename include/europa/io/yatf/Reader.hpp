//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_YATFREADER_H
#define EUROPA_IO_YATFREADER_H

#include <europa/structs/Yatf.hpp>
#include <mco/io/stream.hpp>

namespace europa::util {
	struct ImageSurface;
}

namespace europa::io::yatf {

	/// Reader for PS2 Europa .tex (YATF - Yet Another Texture Format) files.
	struct Reader {
		explicit Reader(mco::Stream& is);

		/// Read image. The image output to [surface] is converted to RGBA8888 implicitly.
		bool ReadImage(structs::YatfHeader& header, util::ImageSurface& surface);

	   private:
		mco::Stream& stream;
	};

} // namespace europa::io::yatf

#endif // EUROPA_IO_YATFREADER_H
