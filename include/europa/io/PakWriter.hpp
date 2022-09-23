//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKWRITER_H
#define EUROPA_IO_PAKWRITER_H

#include <europa/io/PakFile.hpp>
#include <europa/io/PakProgressReportSink.hpp>
#include <iosfwd>
#include <string>
#include <utility>

namespace europa::io {

	/**
	 * Writer for package files.
	 */
	struct PakWriter {
		using FlattenedType = std::pair<std::string, PakFile>;

		void Init(structs::PakHeader::Version version);

		const structs::PakHeader& GetHeader() const { return pakHeader; }

		/**
		 * Write the resulting archive to the given output stream.
		 */
		void Write(std::ostream& os, std::vector<FlattenedType>&& vec, PakProgressReportSink& sink);

	   private:
		structs::PakHeader pakHeader {};
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKWRITER_H
