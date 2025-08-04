//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKWRITER_H
#define EUROPA_IO_PAKWRITER_H

#include <europa/base/VirtualFileSystem.hpp>
#include <europa/io/pak/File.hpp>
#include <europa/io/pak/WriterProgressReportSink.hpp>
#include <europa/structs/Pak.hpp>
#include <string>
#include <utility>

namespace europa::io::pak {

	/// A efficient writer for Europa package (.pak) files.
	struct Writer {
		/// Vocabulary type for making sector alignment stuff a bit easier to see.
		enum class SectorAlignment {
			DoNotAlign, /// Do not align to a sector boundary
			Align		/// Align to a sector boundary
		};

		using FlattenedType = std::pair<std::string, File>;

		/// Manifest data structure.
		struct Manifest {
			explicit Manifest(std::vector<FlattenedType>& files, std::vector<std::string>& tocOrder)
				: files(files), tocOrder(tocOrder) {
			}

			structs::PakVersion version;
			std::uint32_t creationUnixTime;
			SectorAlignment sectorAlignment { SectorAlignment::DoNotAlign };
			std::vector<FlattenedType>& files;
			std::vector<std::string>& tocOrder;
		};

		constexpr Writer() = default;

		/// Initalize for the given package version.
		void SetVersion(structs::PakVersion version);

		/// Write an archive to the stream.
		/// [os] is a VfsFileHandle
		/// [sink] is a implementation of PakProgressReportsSink which should get events (TODO: Make this optional)
		/// [manifest] is the pak manifest
		void Write(base::VfsFileHandle& os, WriterProgressReportSink& sink, const Manifest& manifest);

	   private:
		template <class T>
		void WriteImpl(base::VfsFileHandle& os, WriterProgressReportSink& sink, const Manifest& manifest);

		structs::PakVersion version {};
	};

} // namespace europa::io::pak

#endif // EUROPA_IO_PAKWRITER_H
