//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

#include <europa/structs/Pak.h>

namespace europa::io {


	struct PakReader {

		struct File {
			File(std::vector<std::uint8_t>&& data, structs::PakTocEntry& tocData);

			const std::vector<std::uint8_t>& GetData() const;

			const structs::PakTocEntry& GetTOCEntry() const;

		   private:
			std::vector<std::uint8_t> data;
			structs::PakTocEntry tocData;
		};

		explicit PakReader(std::istream& is);

		void ReadData();


		bool Invalid() const {
			return invalid;
		}

		const std::unordered_map<std::string, File>& GetFiles() const;

	   private:
		std::istream& stream;
		bool invalid{false};

		structs::PakHeader header{};

		std::unordered_map<std::string, structs::PakTocEntry> tocData;
		std::unordered_map<std::string, File> files;
	};



}

#endif // EUROPA_IO_PAKREADER_H
