//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_YATFREADER_H
#define EUROPA_IO_YATFREADER_H

#include <pixel/RgbaImage.h>

#include <europa/structs/Yatf.hpp>
#include <iosfwd>

namespace europa::io::yatf {

	/// Reader for PS2 Europa .tex (YATF - Yet Another Texture Format) files.
	struct Reader {
		explicit Reader(std::istream& is);

		void InitFromStream(std::istream& is);

		void ReadImage();

		pixel::RgbaImage& GetImage();

		const structs::YatfHeader& GetHeader() const;

		[[nodiscard]] bool Invalid() const {
			return invalid;
		}

	   private:
		std::istream& stream;
		bool invalid { false };

		structs::YatfHeader header;
		pixel::RgbaImage image;
	};

} // namespace europa::io

#endif // EUROPA_IO_YATFREADER_H
