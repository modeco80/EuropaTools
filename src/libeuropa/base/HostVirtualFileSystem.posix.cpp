
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include <europa/base/VirtualFileSystem.hpp>
#include <string_view>
#include <system_error>

namespace europa::base {

	namespace {

		/// Helper which converts the VFS open mode to POSIX's mode.
		int openModeToPosixMode(int mode) {
			int a = 0;

			if(mode & VirtualFileSystem::Read)
				a |= O_RDONLY;
			if(mode & VirtualFileSystem::Write)
				a |= O_WRONLY;

			if(mode & VirtualFileSystem::Create)
				a |= O_CREAT;

			return a;
		}

		struct HostVfsFile : VfsFile {
			HostVfsFile()
				: VfsFile() {
			}

			void OpenFileImpl(std::error_code& ec, const std::string_view path, int mode) {
				auto modeConverted = openModeToPosixMode(mode);

				// This looks dumb but we basically have to handle this this way when creating new files,
				// since it always expects the file's mode to be provided. We just provide a "good enough"
				// blanket on which Works.
				if(modeConverted & O_CREAT)
					fd = open(path.data(), modeConverted, 0666);
				else
					fd = open(path.data(), modeConverted);

				if(fd == -1) {
					ec = { errno, std::system_category() };
					return;
				}

				ec = {};
				return;
			}

			void Close() override {
				if(fd) {
					close(fd);
					fd = -1;
				}
			}

			VfsFile* Clone() override {
#if defined(__linux__)
				// This is Linux-specific but it allows us to open a new file
				// without needing to hold the filename needlessly.
				char path[32] {};
				snprintf(&path[0], sizeof(path) - 1, "/proc/self/fd/%d", fd);

				// This is a little :( munging around with internal state like this, but
				// this saves a round trip through the mode conversion code when we know
				// we only want O_RDONLY when cloning (since it'd be unsafe to allow clones to write).
				auto* file = new HostVfsFile();
				file->fd = open(path, O_RDONLY);

				if(fd == -1) {
					delete file;
					throw std::system_error(std::error_code { errno, std::system_category() });
				}

				return file;
#else
	// FIXME: a POSIX-compatible implementation of this would be nice.
	#error This code currently only supports Linux.
#endif
			}

			void Truncate(std::uint64_t newSize) override {
				ftruncate64(fd, newSize);
			}

			std::uint64_t Tell() const override {
				return lseek(fd, 0, SEEK_CUR);
			}

			std::uint64_t Seek(std::error_code& ec, std::int64_t offset, SeekDirection whence = RelativeBegin) override {
				int dir = 0;
				switch(whence) {
					case VfsFile::RelativeBegin:
						dir = SEEK_SET;
						break;
					case VfsFile::RelativeCurrent:
						dir = SEEK_CUR;
						break;
					case VfsFile::RelativeEnd:
						dir = SEEK_END;
						break;
				}

				auto res = lseek(fd, offset, dir);
				if(res == -1) {
					ec = { errno, std::system_category() };
					return res;
				}

				return res;
			}

			std::uint64_t Read(std::error_code& ec, std::uint8_t* pBuffer, std::size_t length) override {
				auto res = read(fd, pBuffer, length);
				if(res == -1) {
					ec = { errno, std::system_category() };
					return 0;
				}

				return res;
			}

			std::uint64_t Write(std::error_code& ec, const std::uint8_t* pBuffer, std::size_t length) override {
				auto res = write(fd, pBuffer, length);
				if(res == -1) {
					ec = { errno, std::system_category() };
					return 0;
				}
				return res;
			}

		   private:
			int fd { -1 };
		};

		struct HostVfs : VirtualFileSystem {
			const char* GetName() const override {
				return "Host";
			}

			VfsFileHandle Open(std::error_code& ec, const std::string_view path, int mode = Read) override {
				auto* pFile = new HostVfsFile();

				// Do the actual file open. If this fails, we'll just give up.
				pFile->OpenFileImpl(ec, path, mode);

				if(ec) {
					delete pFile;
					return VfsFileHandle(nullptr);
				}

				return VfsFileHandle(pFile);
			}

			StatFile Stat(const std::string_view path) override {
				struct stat statBuffer {};
				if(stat(path.data(), &statBuffer) == -1) {
					throw std::system_error(std::error_code { errno, std::system_category() });
				}

				auto sf = VirtualFileSystem::StatFile();
				sf.lengthBytes = statBuffer.st_size;
				return sf;
			}

			StatFileSystem StatFS() override {
				// Nothing implemented for this.
				return {};
			}
		};

		static HostVfs gHostFileSystem;
	} // namespace

	VirtualFileSystem& HostFileSystem() {
		return gHostFileSystem;
	}
} // namespace europa::base