//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <europa/io/PakFile.hpp>
#include <europa/structs/Pak.hpp>
#include <iosfwd>
#include <string>
#include <map>

namespace europa::io {

	/// Reader for Europa package files (.pak).
	struct PakReader {
		using FlatType = std::pair<std::string, PakFile>;
		using MapType = std::vector<FlatType>;

		/// Constructor. Takes in a input stream to read pak data from.
		/// This stream should only be used by the PakReader, nothing else.
		explicit PakReader(std::istream& is);

		/// Reads the header and the file TOC.
		/// This function should be called first.
		void ReadHeaderAndTOC();

		/// Reads all files in the package.
		void ReadFiles();

		/// Reads a file with the path specified as [file].
		void ReadFile(const std::string& file);

		bool Invalid() const {
			return invalid;
		}

		MapType& GetFiles();
		const MapType& GetFiles() const;

		// implement in cpp later, lazy and just wanna get this out :vvv
		const structs::PakHeaderVariant& GetHeader() const {
			return header;
		}

	   private:
		template <class T>
		void ReadData_Impl();

		std::istream& stream;
		bool invalid { false };

		structs::PakVersion version;
		structs::PakHeaderVariant header {};

		MapType files;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKREADER_H
