//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <europa/io/pak/File.hpp>
#include <europa/structs/Pak.hpp>
#include <europa/util/DllExport.hpp>
#include <iosfwd>
#include <string>

namespace europa::io::pak {

	/// Reader for Europa package files (.pak).
	struct LIBEUROPA_EXPORT Reader {
		using FlatType = std::pair<std::string, File>;
		using MapType = std::vector<FlatType>;

		/// Constructor. Takes in a input stream to read pak data from.
		/// This stream should only be used by the PakReader, nothing else.
		explicit Reader(std::istream& is);

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

		const structs::PakHeaderVariant& GetHeader() const {
			return header;
		}

	   private:
		template <class T>
		void ReadHeaderAndTOCImpl();

		std::istream& stream;
		bool invalid { false };

		structs::PakHeaderVariant header {};
		MapType files;
	};

} // namespace europa::io::pak

#endif // EUROPA_IO_PAKREADER_H
