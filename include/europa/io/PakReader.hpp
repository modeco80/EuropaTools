//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <europa/io/PakFile.hpp>
#include <europa/structs/Pak.hpp>
#include <iosfwd>
#include <string>
#include <unordered_map>

#include <variant>

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

		// implement in cpp later, lazy and just wanna get this out :vvv
		const structs::PakHeaderVariant& GetHeader() const { return header; }

	   private:
	   	template<class T>
		void ReadData_Impl();

		std::istream& stream;
		bool invalid { false };

		structs::PakVersion version;
		structs::PakHeaderVariant header {};

		MapType files;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKREADER_H
