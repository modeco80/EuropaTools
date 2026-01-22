//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstddef>
#include <mco/base_types.hpp>
#include <memory>

namespace europa::util {

	/// A little ergonomic wrapper over
	/// std::unique_ptr<T[]> for buffers
	/// that need to track their size as well.
	template <class T>
	struct UniqueArray final {
		UniqueArray() = default;
		explicit UniqueArray(usize size) {
			resize(size);
		}

		UniqueArray(const UniqueArray&) = delete;

		UniqueArray(UniqueArray&& move) {
			array = std::move(move.array);
			mSize = move.mSize;

			// invalidate moved-from array to default state
			move.array = nullptr;
			move.mSize = 0;
		}

		UniqueArray& operator=(UniqueArray&& move) {
			array = std::move(move.array);
			mSize = move.mSize;

			// invalidate moved-from array to default state
			move.array = nullptr;
			move.mSize = 0;
			return *this;
		}

		void resize(usize size) {
			this->array = std::make_unique<T[]>(size);
			this->mSize = size;
		}

		/// Clears this array.
		void clear() {
			this->array.reset();
			this->mSize = 0;
		}

		T& operator[](usize index) {
			return (array.get())[index];
		}

		const T& operator[](usize index) const {
			return (array.get())[index];
		}

		T* data() {
			return array.get();
		}

		const T* data() const {
			return array.get();
		}

		usize size() const {
			return mSize;
		}

	   private:
		std::unique_ptr<T[]> array {};
		usize mSize {};
	};
} // namespace europa::util
