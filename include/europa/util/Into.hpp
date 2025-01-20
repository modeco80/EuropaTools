#pragma once

#include <vector>

namespace europa::util {

	template <class To, class From>
	struct IntoImpl {
		constexpr static To DoInto(const From& from) {
			// The default implentation assumes that the To type has a constructor which
			// can initalize it from a value of From const&.
			return To(from);
		}
	};

	/// Into() support for vectors of items.
	template <class To, class From>
	struct IntoImpl<std::vector<To>, std::vector<From>> {
		constexpr static std::vector<To> DoInto(const std::vector<From>& from) {
			// FIXME: Ranges could make this less stupid
			auto vec = std::vector<To> {};
			vec.resize(from.size());

			// Feed into existing Into() implementation
			for(auto const& item : from)
				vec.push_back(Into(item));

			return vec;
		}
	};

	/// Allows a type to be converted.
	/// A user is allowed to supply conversions out-of-band.
	template <class To, class From>
	constexpr To Into(const From& from) {
		return IntoImpl<To, From>::DoInto(from);
	}

/// Helper macro to hide away the annoying boilerplate of
/// implementing the IntoImpl struct used for implementing conversion
#define EUROPA_BEGIN_INTO_CONVERSION(To, From)  \
	template <>                                 \
	struct ::europa::util::IntoImpl<To, From> { \
		static To DoInto([[maybe_unused]] const From& from)

#define EUROPA_END_INTO_CONVERSION }

#if 0
    // An example of specializing Into().
    // This is a bit iffy, but works.
    struct Frob {};

    EUROPA_BEGIN_INTO_CONVERSION(int, Frob) {
           return 42;
    } EUROPA_END_INTO_CONVERSION;

    auto a() {
        Frob f{};
        int i = Into<int>(f);
    }
#endif

} // namespace europa::util