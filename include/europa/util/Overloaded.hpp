//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_UTIL_OVERLOADED_HPP
#define EUROPA_UTIL_OVERLOADED_HPP

namespace europa {
	template <class... Ts>
	struct overloaded : Ts... {
		using Ts::operator()...;
	};

	// Suppposedly this isn't needed but the CTAD is required in this case

	template <class... Ts>
	overloaded(Ts...) -> overloaded<Ts...>;
} // namespace europa

#endif