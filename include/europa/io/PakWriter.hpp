//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKWRITER_H
#define EUROPA_IO_PAKWRITER_H

#include <europa/io/PakFile.hpp>
#include <europa/io/PakProgressReportSink.hpp>
#include <iosfwd>
#include <string>
#include <utility>

#include "europa/structs/Pak.hpp"

namespace europa::io {

	/// A efficient writer for Europa package (.pak) files.
	struct PakWriter {
		/// Vocabulary type for making sector alignment stuff a bit easier to see.
		enum class SectorAlignment {
			DoNotAlign, /// Do not align to a sector boundary
			Align		/// Align to a sector boundary
		};

		using FlattenedType = std::pair<std::string, PakFile>;

		/// Initalize for the given package version.
		void SetVersion(structs::PakVersion version);

		/// Write archive to the given output stream.
		/// [vec] is all files which should be packaged
		/// [sink] is a implementation of PakProgressReportsSink which should get events (TODO: Make this optional)
		/// [sectorAlignment] controls sector alignment
		void Write(std::ostream& os, std::vector<FlattenedType>&& vec, PakProgressReportSink& sink, SectorAlignment sectorAlignment = SectorAlignment::DoNotAlign);

	   private:
		template <class T>
		void WriteImpl(std::ostream& os, std::vector<FlattenedType>&& vec, PakProgressReportSink& sink, SectorAlignment sectorAlignment);

		structs::PakVersion version {};
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKWRITER_H
