//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKWRITER_H
#define EUROPA_IO_PAKWRITER_H

#include <europa/io/PakFile.h>

#include <iosfwd>
#include <string>
#include <unordered_map>

namespace europa::io {

	/**
	 * Writer for package files.
	 */
	struct PakWriter {
		void Init(structs::PakVersion version);

		std::unordered_map<std::string, PakFile>& GetFiles();

		/**
		 * Write the resulting archive to the given output stream.
		 */
		void Write(std::ostream& os);

	   private:
		structs::PakHeader pakHeader {};
		std::unordered_map<std::string, PakFile> archiveFiles;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKWRITER_H