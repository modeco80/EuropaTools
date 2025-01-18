//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#ifdef europa_EXPORTS
	#ifdef _WIN32
		#define LIBEUROPA_EXPORT __declspec(dllexport)
	#else
		#define LIBEUROPA_EXPORT __attribute__((visibility("default")))
	#endif
#else
	#ifdef _WIN32
		#define LIBEUROPA_EXPORT __declspec(dllimport)
	#else
		#define LIBEUROPA_EXPORT
	#endif
#endif