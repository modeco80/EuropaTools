//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <europa/base/VirtualFileSystem.hpp>
#include <system_error>

namespace europa::base {

	std::uint64_t VfsFile::Seek(std::int64_t offset, SeekDirection whence) {
		std::error_code ec {};
		auto res = Seek(ec, offset, whence);
		if(ec)
			throw std::system_error(ec);
		return res;
	}

	std::uint64_t VfsFile::Read(std::error_code& ec, std::uint8_t* pBuffer, std::size_t length) {
		static_cast<void>(pBuffer);
		static_cast<void>(length);
		// no error, just nothing read.
		ec = {};
		return 0;
	}

	std::uint64_t VfsFile::Write(std::error_code& ec, const std::uint8_t* pBuffer, std::size_t length) {
		static_cast<void>(pBuffer);
		static_cast<void>(length);
		ec = {};
		return 0;
	}

	std::uint64_t VfsFile::Read(std::uint8_t* pBuffer, std::size_t length) {
		std::error_code ec {};
		auto res = Read(ec, pBuffer, length);
		if(ec)
			throw std::system_error(ec);
		return res;
	}

	std::uint64_t VfsFile::Write(const std::uint8_t* pBuffer, std::size_t length) {
		std::error_code ec {};
		auto res = Write(ec, pBuffer, length);
		if(ec)
			throw std::system_error(ec);
		return res;
	}

	VfsFileHandle VirtualFileSystem::Open(const std::string_view path, int mode) {
		std::error_code ec;
		auto res = Open(ec, path, mode);
		if(ec)
			throw std::system_error(ec);
		return res;
	};

} // namespace europa::base