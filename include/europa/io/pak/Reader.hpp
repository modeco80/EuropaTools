//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <mco/io/stream.hpp>
#include <europa/io/pak/File.hpp>
#include <europa/structs/Pak.hpp>
#include <string>

namespace europa::io::pak {

	/// Low-level reader for Europa package files (.pak).
	struct Reader {
		using FlatType = std::pair<std::string, File>;
		using MapType = std::vector<FlatType>;

		// TODO (since I'll need to add it in mcolib itself):
		// OpenedFile at a later time will have to clone the stream
		// to stay concurrent-safe. For now, this doesn't matter,
		// but for later (e.g: threaded extraction, so on...), it
		// is something to note

		/// An opened package file.
		struct OpenedFile : mco::Stream {
			// Stream
			bool isRandomAccess() const noexcept override;

			u64 read(void* buffer, u64 length) override;

			i64 tell() override;
			i64 seek(i64 offset, Whence whence) override;
			u64 getSize() override;

		private:
			friend Reader;
			OpenedFile(mco::Stream& source, File& file);

			mco::Stream& source;
			File& file;
			std::size_t virtualPosition;
		};

		/// Constructor. Takes in a input stream to read pak data from.
		/// This stream should only be used by the PakReader, nothing else.
		explicit Reader(mco::Stream& is);

		/// Initialize the package reader.
		void init();

		/// Opens a file at [path].
		OpenedFile open(const std::string& path);

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

		mco::Stream& stream;
		bool invalid { false };

		structs::PakHeaderVariant header {};
		MapType files;
	};

} // namespace europa::io::pak

#endif // EUROPA_IO_PAKREADER_H
