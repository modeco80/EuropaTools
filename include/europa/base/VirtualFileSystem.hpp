//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

// This file contains the base declarations for the libeuropa VFS.
//
// This is the API that all libeuropa functions doing I/O on files are expected to use,
// to stay FS-independent.

#pragma once

#include <cstdint>
#include <europa/util/RelocatabilityMarkers.hpp>
#include <stdexcept>
#include <string_view>
#include <system_error>

#include "europa/base/MoveOnlyAny.hpp"

namespace europa::base {

	struct VirtualFileSystem; // fwd declare for later!

	/// A VFS file.
	struct VfsFile {
		VfsFile() = default;
		EUROPA_FORBID_RELOCATION(VfsFile);

		enum SeekDirection {
			RelativeBegin,
			RelativeCurrent,
			RelativeEnd
		};

		virtual ~VfsFile() = default;

		virtual void Close() = 0;

		/// Clones a new file.
		virtual VfsFile* Clone() = 0;

		virtual void Truncate(std::uint64_t newSize) = 0;

		virtual std::uint64_t Tell() const = 0;

		virtual std::uint64_t Seek(std::error_code& ec, std::int64_t offset, SeekDirection whence = RelativeBegin) = 0;

		std::uint64_t Seek(std::int64_t offset, SeekDirection whence = RelativeBegin);

		/// Reads some data from this file.
		virtual std::uint64_t Read(std::error_code& ec, std::uint8_t* pBuffer, std::size_t length);

		std::uint8_t Get() {
			std::uint8_t c{};
			Read(&c, 1);
			return c;
		}

		/// Writes some data.
		virtual std::uint64_t Write(std::error_code& ec, const std::uint8_t* pBuffer, std::size_t length);

		std::uint64_t Read(std::uint8_t* pBuffer, std::size_t length);

		std::uint64_t Write(const std::uint8_t* pBuffer, std::size_t length);
	};

	/// A RAII-safe handle to a VFS file.
	struct VfsFileHandle {
		explicit VfsFileHandle(VfsFile* pFile)
			: raw(pFile) {
		}

		EUROPA_FORBID_COPY(VfsFileHandle);

		VfsFileHandle(VfsFileHandle&& mv)
			: raw(mv.raw) {
			mv.raw = nullptr;
		}

		~VfsFileHandle() {
			Close();
		}

		void Close() {
			if(raw) {
				raw->Close();
				delete raw;
				raw = nullptr;
			}
		}

		/// Clones this file to a new file.
		VfsFileHandle Clone() {
			return VfsFileHandle(raw->Clone());
		}

		operator bool() {
			return raw != nullptr;
		}

		VfsFile* operator->() {
			return raw;
		}

		VfsFile& operator*() {
			return *raw;
		}

	   private:
		VfsFile* raw;
	};

	/// A mounted VFS filesystem.
	struct VirtualFileSystem {
		struct StatFile {
			StatFile() = default;
			EUROPA_FORBID_COPY(StatFile);
			EUROPA_ALLOW_MOVE(StatFile);

			bool HasXAttr() const {
				return extendedAttr.HasValue();
			}

			std::uint64_t lengthBytes;

			/// Extended filesystem-specific attributes.
			/// The underlying type of this data will be specific
			/// to the filesystem that was statted.
			MoveOnlyAny extendedAttr;
		};

		struct StatFileSystem {
			StatFileSystem() = default;
			EUROPA_FORBID_COPY(StatFileSystem);
			EUROPA_ALLOW_MOVE(StatFileSystem);

			bool HasXAttr() const {
				return extendedAttr.HasValue();
			}

			/// Extended filesystem-specific attributes.
			/// The underlying type of this data will be specific
			/// to the filesystem that was statted.
			MoveOnlyAny extendedAttr;
		};

		enum OpenMode {
			Read = (1 << 0),
			Write = (1 << 1),
			Create = (1 << 2)
		};

		/// Gets the name of this filesystem.
		virtual const char* GetName() const = 0;

		/// Opens a file.
		virtual VfsFileHandle Open(std::error_code& ec, const std::string_view path, int mode = Read) = 0;

		/// Opens a file.
		VfsFileHandle Open(const std::string_view path, int mode = Read);

		// TODO: These should have error_code& overloads too. Also, probably make these `void`, and take a Stat(File|FileSystem)&?

		virtual StatFile Stat(const std::string_view path) = 0;

		virtual StatFileSystem StatFS() = 0;
	};

	/// Get the singleton instance of the host virtual filesystem.
	VirtualFileSystem& HostFileSystem();

} // namespace europa::base