#include <sys/stat.h>

#include <cstdio>
#include <europa/base/VirtualFileSystem.hpp>
#include <string_view>
#include <system_error>

namespace europa::base {

	namespace {

		std::string openModeToLibcMode(int mode) {
			std::string a;

			if(mode & VirtualFileSystem::Read)
				a += 'r';
			if(mode & VirtualFileSystem::Write)
				a += "w+";
#ifdef _WIN32
			a += 'b';
#endif
			return a;
		}

		struct HostVfsFile : VfsFile {
			HostVfsFile()
				: VfsFile(), fp(nullptr) {
			}

			void OpenFileImpl(std::error_code& ec, const std::string_view path, int mode) {
				auto modeConverted = openModeToLibcMode(mode);
				fp = fopen(path.data(), modeConverted.c_str());

				if(fp == nullptr) {
					ec = { errno, std::system_category() };
					return;
				}

				ec = {};
				return;
			}

			void Close() override {
				if(fp) {
					fclose(fp);
					fp = nullptr;
				}
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

				auto res = fseek(fp, offset, dir);
				if(res == -1) {
					ec = { errno, std::system_category() };
					return res;
				}

				return res;
			}

			std::uint64_t Tell() const override {
				return ftell(fp);
			}

			std::uint64_t Read(std::error_code& ec, std::uint8_t* pBuffer, std::size_t length) override {
				auto res = fread(pBuffer, 1, length, fp);
				if(res != length) {
					if(res == static_cast<std::size_t>(EOF)) {
						// EOF. No error.
						if(feof(fp))
							return 0;

						if(ferror(fp)) {
							ec = { errno, std::system_category() };
							return 0;
						}
					}
				}

				return res;
			}

			std::uint64_t Write(std::error_code& ec, const std::uint8_t* pBuffer, std::size_t length) override {
				auto res = fwrite(pBuffer, 1, length, fp);
				if(res != length) {
					if(feof(fp))
						return res;
					ec = { errno, std::system_category() };
				}

				return res;
			}

		   private:
			FILE* fp;
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