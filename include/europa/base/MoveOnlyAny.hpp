//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//
#pragma once

#include <europa/util/RelocatabilityMarkers.hpp>
#include <stdexcept>

namespace europa::base {

	/// A very simple implementation of `std::any`.
	/// This implementation is very type-unsafe, but it works.
	struct MoveOnlyAny {
		MoveOnlyAny() = default;
		EUROPA_FORBID_COPY(MoveOnlyAny);

		template <class T>
		explicit MoveOnlyAny(const T& sourceCopy) {
			CopyBase(&AnyControlImpl<T>, &sourceCopy);
		}

		constexpr MoveOnlyAny(MoveOnlyAny&& mv)
			: pAnyPtr(mv.pAnyPtr), control(mv.control) {
            // Reset the moved-from object to feel like a default-constructed
            // MoveOnlyAny. It can be used again if you like.
			mv.pAnyPtr = nullptr;
			mv.control = nullptr;
		}

		~MoveOnlyAny();

		bool HasValue() const;

		template <class T>
		void InitValue() {
            CreateBase(&AnyControlImpl<T>);
		}

		template <class T>
		T& Get() {
			// TODO: analog bad_any_access exception..
			if(!HasValue())
				throw std::runtime_error("no value in MoveOnlyAny");
			return *reinterpret_cast<T*>(this->pAnyPtr);
		}

	   private:

		// Instead of using a object with vtable, we just use a single routine
		// which can be commandeered by calling it. It's a bit messy, and C-esque, but
		// the beauty of C++ is that this can all be Private Implementation Details.

		enum class ControlOp {
			Create,
			CopyFromSource,
			Delete
		};

		struct CopyFromSourceArgs {
			void* pSourceObject;
		};

		using ControlRoutine = void (*)(MoveOnlyAny* pAnyObject, ControlOp operation, void* argp);

        
        void CreateBase(ControlRoutine rt);
        void CopyBase(ControlRoutine rt, void* pCopyObject);
		void DestroyValue();

		template <class T>
		static void AnyControlImpl(MoveOnlyAny* self, ControlOp op, void* argp) {
			switch(op) {
				case ControlOp::Create:
					self->pAnyPtr = new T();
					break;
				case ControlOp::CopyFromSource: {
					auto pArg = reinterpret_cast<CopyFromSourceArgs*>(argp);
					self->pAnyPtr = new T(*reinterpret_cast<const T*>(pArg->pSourceObject));
				} break;
				case ControlOp::Delete:
					delete reinterpret_cast<T*>(self->pAnyPtr);
					self->pAnyPtr = nullptr;
					break;
			}
		}

		void* pAnyPtr = nullptr;
		ControlRoutine control = nullptr;
	};
} // namespace europa::base