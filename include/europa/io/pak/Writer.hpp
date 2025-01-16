//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKWRITER_H
#define EUROPA_IO_PAKWRITER_H

#include <europa/io/pak/File.hpp>
#include <europa/io/pak/WriterProgressReportSink.hpp>
#include <iosfwd>
#include <string>
#include <utility>

#include "europa/structs/Pak.hpp"

namespace europa::io::pak {

	/// A efficient writer for Europa package (.pak) files.
	struct Writer {
		/// Vocabulary type for making sector alignment stuff a bit easier to see.
		enum class SectorAlignment {
			DoNotAlign, /// Do not align to a sector boundary
			Align		/// Align to a sector boundary
		};

		using FlattenedType = std::pair<std::string, File>;

		constexpr Writer() = default;

		Writer(structs::PakVersion version) {
			SetVersion(version);
		}

		/// Initalize for the given package version.
		void SetVersion(structs::PakVersion version);

		/// Write archive to the given output stream.
		/// [vec] is all files which should be packaged
		/// [sink] is a implementation of PakProgressReportsSink which should get events (TODO: Make this optional)
		/// [sectorAlignment] controls sector alignment. It is ignored unless the package's version is [structs::PakVersion::Ver5].
		void Write(std::ostream& os, std::vector<FlattenedType>&& vec, WriterProgressReportSink& sink, SectorAlignment sectorAlignment = SectorAlignment::DoNotAlign);

	   private:
		template <class T>
		void WriteImpl(std::ostream& os, std::vector<FlattenedType>&& vec, WriterProgressReportSink& sink, SectorAlignment sectorAlignment);

		structs::PakVersion version {};
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKWRITER_H
