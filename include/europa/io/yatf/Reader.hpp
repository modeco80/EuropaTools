//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_YATFREADER_H
#define EUROPA_IO_YATFREADER_H

#include <europa/structs/Yatf.hpp>
#include <europa/util/DllExport.hpp>
#include <iosfwd>

namespace europa::util {
	struct ImageSurface;
}

namespace europa::io::yatf {

	/// Reader for PS2 Europa .tex (YATF - Yet Another Texture Format) files.
	struct LIBEUROPA_EXPORT Reader {
		explicit Reader(std::istream& is);

		/// Read image. The image output to [surface] is converted to RGBA8888 implicitly.
		bool ReadImage(structs::YatfHeader& header, util::ImageSurface& surface);

	   private:
		std::istream& stream;
	};

} // namespace europa::io::yatf

#endif // EUROPA_IO_YATFREADER_H
