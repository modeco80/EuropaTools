#!/bin/bash

set -xeuo pipefail

# configure & build europatools
[[ ! -d 'buildwin' ]] && {
	cmake -Wno-dev \
		-Bbuildwin/ \
		-GNinja \
		--toolchain ~/mingw64.cmake \
		-DCMAKE_BUILD_TYPE=Release
};

ninja -C buildwin

# Make a simple version file.
echo "Version: $(git describe --dirty)" > /tmp/VERSION.txt
echo "Git commit: $(git rev-parse HEAD)" >> /tmp/VERSION.txt
echo "Git branch: $(git branch --show-current)" >> /tmp/VERSION.txt
dos2unix /tmp/VERSION.txt

# Now, make the package.
7z a -mx9 -mmtf=on -mmt13 europatools-win64.7z \
	/usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll \
	/usr/x86_64-w64-mingw32/bin/libstdc++-6.dll \
	/usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll \
	$PWD/buildwin/eupak.exe \
	$PWD/buildwin/eutex.exe \
	$PWD/buildwin/jsfscramble.exe\
	doc/eupak.md \
	doc/eutex.md \
	README.md \
	/tmp/VERSION.txt

rm /tmp/VERSION.txt
