//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKFILE_H
#define EUROPA_IO_PAKFILE_H

#include <europa/structs/Pak.h>

#include <cstdint>
#include <vector>

namespace europa::io {

	struct PakReader;
	struct PakWriter;

	struct PakFile {
		using DataType = std::vector<std::uint8_t>;

		/**
		 * Get the file data.
		 */
		[[nodiscard]] const DataType& GetData() const;

		/**
		 * Get the TOC entry responsible.
		 */
		[[nodiscard]] const structs::PakTocEntry& GetTOCEntry() const;

		void SetData(DataType&& data);

		structs::PakTocEntry& GetTOCEntry();

		void FillTOCEntry();

	   private:
		friend PakReader;
		friend PakWriter;

		std::vector<std::uint8_t> data;
		structs::PakTocEntry tocData;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKFILE_H
