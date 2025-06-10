#include <cstdio>
#include <europa/base/VirtualFileSystem.hpp>

// Test program for virtual file system functionality

namespace ebase = europa::base;

/// Tests that a vfs file handle can be moved, and
/// that it is not rugpulled (i.e: ownership truthfully moved.)
void testMove(ebase::VfsFileHandle&& mv) {
    auto movedHandle = std::move(mv);

	char msg[] = "A file handle can successfuly be moved to different ownership.\n";
	auto nWritten = (*movedHandle).Write(reinterpret_cast<const std::uint8_t*>(&msg[0]), sizeof(msg) - 1);
	if(nWritten != (sizeof(msg) - 1)) {
		printf("Short write\n");
	}
}

int main() {
	auto& hostFs = ebase::HostFileSystem();
    
	auto handle = hostFs.Open("a.txt", ebase::VirtualFileSystem::Read | ebase::VirtualFileSystem::Write);

	char msg[] = "Hello World.\n";
	auto nWritten = (*handle).Write(reinterpret_cast<const std::uint8_t*>(&msg[0]), sizeof(msg) - 1);
	if(nWritten != (sizeof(msg) - 1)) {
		printf("Short write\n");
		return 1;
	}

    testMove(std::move(handle));

	return 0;
}