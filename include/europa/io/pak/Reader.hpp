//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <europa/base/VirtualFileSystem.hpp>
#include <europa/io/pak/File.hpp>
#include <europa/structs/Pak.hpp>
#include <string>

namespace europa::io::pak {

	/// Low-level reader for Europa package files (.pak).
	struct Reader {
		using FlatType = std::pair<std::string, File>;
		using MapType = std::vector<FlatType>;

		/// Wrapper over VfsFileHandle which allows reading a pack file as a stream
		struct OpenedPackageFile {
			/// Seek in the package file.
			std::uint64_t Seek(std::int64_t offset, base::VfsFile::SeekDirection whence);

			/// Tell.
			std::uint64_t Tell();

			/// Read some data.
			std::uint64_t Read(std::uint8_t* pBuffer, std::size_t bufferLength);
			
		   private:
		   friend Reader;
		    OpenedPackageFile(base::VfsFileHandle&& clonedFile);
			void Init(File& data);
			base::VfsFileHandle fh;
			std::size_t fileOffset;
			std::size_t fileSize;
			std::size_t virtualPos = 0;
		};

		/// Constructor. Takes in a input stream to read pak data from.
		/// This stream should only be used by the PakReader, nothing else.
		explicit Reader(base::VfsFileHandle&& is);

		/// Reads the header and the file TOC.
		/// This function should be called first before doing any other operations
		/// on the reader.
		void ReadHeaderAndTOC();

		/// Opens a file with the path specified as [file].
		/// Throws if the file does not exist.
		OpenedPackageFile Open(const std::string& file);

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

		base::VfsFileHandle stream;
		bool invalid { false };

		structs::PakHeaderVariant header {};
		MapType files;
	};

} // namespace europa::io::pak

#endif // EUROPA_IO_PAKREADER_H
