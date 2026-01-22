//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKREADER_H
#define EUROPA_IO_PAKREADER_H

#include <europa/io/pak/File.hpp>
#include <europa/structs/Pak.hpp>
#include <mco/io/stream.hpp>
#include <string>

namespace europa::io::pak {

	/// Reader for Europa package files (.pak). Provides a mcolib stream for
	/// opened package files.
	struct Reader {
		using FlatType = std::pair<std::string, File>;
		using MapType = std::vector<FlatType>;

		// TODO (since I'll need to add it in mcolib itself):
		// OpenedFile at a later time will have to clone the stream
		// to stay concurrent-safe. For now, this doesn't matter,
		// but for later (e.g: threaded extraction, so on...), it
		// is something to note. By that point it'll probably have to be
		// a Ref<Stream> ? At least that's what I'll have Stream::clone() return.

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
			usize virtualPosition;
		};

		/// Constructor. Takes in a input stream to read pak data from.
		/// This stream should only be used by the PakReader, nothing else.
		explicit Reader(mco::Stream& is);

		/// Initialize the package reader.
		void init();

		/// Opens a file at [path].
		OpenedFile open(const std::string& path);

		/// Gets const access to all package files.
		const MapType& getPackageFiles() const;

		/// Gets const access to the package header.
		const structs::PakHeaderVariant& getPackageHeader() const {
			return header;
		}

	   private:
		template <class T>
		void initImpl();

		mco::Stream& stream;

		structs::PakHeaderVariant header {};
		MapType files;
	};

} // namespace europa::io::pak

#endif // EUROPA_IO_PAKREADER_H
