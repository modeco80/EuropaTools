//
// SSX 3 Lobby Server
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// This file is licensed under the GNU General Public License Version 3.
// Text is provided in LICENSE.
//

#ifndef EUROPA_UTIL_TUPLEELEMENT_H
#define EUROPA_UTIL_TUPLEELEMENT_H

#include <cstdint>
#include <cstddef>

#include <tuple>

namespace europa::util {

	namespace detail {

		template<std::size_t N>
		struct TupleElementImpl {
			template<typename T>
			constexpr decltype(auto) operator()(T&& t) const {
				using std::get;
				return get<N>(std::forward<T>(t));
			}
		};

		template<std::size_t N>
		inline constexpr TupleElementImpl<N> TupleElement;
	}

	using detail::TupleElement;
}

#endif // EUROPA_UTIL_TUPLEELEMENT_H
