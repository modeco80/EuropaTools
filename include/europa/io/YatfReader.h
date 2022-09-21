//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef EUROPA_IO_YATFREADER_H
#define EUROPA_IO_YATFREADER_H

#include <europa/structs/Yatf.h>
#include <pixel/RgbaImage.h>

#include <iosfwd>

namespace europa::io {

	/**
	 * Reader for PS2 Europa .tex files.
	 */
	struct YatfReader {
		explicit YatfReader(std::istream& is);

		void Init(std::istream& is);

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

		/**
		 * converted image.
		 */
		pixel::RgbaImage image;
	};

} // namespace europa::io

#endif // EUROPA_IO_YATFREADER_H
