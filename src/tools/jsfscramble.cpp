//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

// A utility to scramble strings.

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

/// Define this to enable an "interactive" mode where the scramble routine will
/// dump out whatever it does to standard out. Undefine this to make it not do so.
#define INTERACTIVE

/// Define this to use a slower, more academic implementation of the algorithm.
// #define SLOW

#ifdef INTERACTIVE
	// We need to use the
	#ifndef SLOW
		#define SLOW
	#endif
#endif

#ifdef SLOW
template <typename... Args>
inline std::string StringPrintf(std::string_view format, Args&&... args) {
	char buffer[1024];
	auto len = std::snprintf(&buffer[0], sizeof(buffer) - 1, format.data(), static_cast<Args&&>(args)...);
	if(len == -1)
		return "";
	buffer[len] = '\0';
	return { buffer, static_cast<std::size_t>(len) };
}

std::uint32_t swsfScramble(const std::string& code) {
	// The input string needs to be processed before actually scrambling:
	// - prepend "code_" to it
	// - make it lowercase
	auto copy = StringPrintf("code_%s", code.c_str());
	std::uint32_t tally {};
	for(auto& c : copy)
		c = tolower(c);

	#ifdef INTERACTIVE
	std::printf("working with string \"%s\"\n", copy.c_str());
	#endif

	// Now actually do the scramble. The algorithm is pretty simple.
	for(auto* p = copy.data(); *p; ++p) {
	#ifdef INTERACTIVE
		std::uint32_t clone = *p;
		std::printf("load character '%c' -> 0x%08x (%d)\n", *p, clone, clone);

		clone += tally * 5;
		std::printf("add (0x%08x (%d) * 5) (%d) -> 0x%08x (%d)\n", tally, tally, tally * 5, clone, clone);

		tally = clone;
	#else
		tally = *p + (tally * 5);
	#endif
	}

	return tally;
}

#else
	#error FIXME: Import fast/optimized version
#endif

int main(int argc, char** argv) {
	if(argc < 2) {
		std::printf("usage: %s [code-string]\n", argv[0]);
		return 0;
	}

	auto result = swsfScramble(argv[1]);
	std::printf("scrambled is: 0x%08x (%d)\n", result, result);
}