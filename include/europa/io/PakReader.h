//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <europa/io/PakFile.h>
#include <europa/structs/Pak.h>

#include <iosfwd>
#include <string>
#include <unordered_map>

namespace europa::io {

	struct PakReader {
		using MapType = std::unordered_map<std::string, PakFile>;

		explicit PakReader(std::istream& is);

		void ReadData();

		void ReadFiles();

		/**
		 * Read in a specific file.
		 */
		void ReadFile(const std::string& file);

		bool Invalid() const {
			return invalid;
		}

		MapType& GetFiles();
		const MapType& GetFiles() const;

	   private:
		std::istream& stream;
		bool invalid { false };

		structs::PakHeader header {};

		MapType files;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKREADER_H
