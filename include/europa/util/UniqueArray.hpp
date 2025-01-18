//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once

#include <cstddef>
#include <memory>
#include <europa/util/DllExport.hpp>

namespace europa::util {

	/// A little ergonomic wrapper over
	/// std::unique_ptr<T[]> for buffers
	/// that need to track their size as well.
	template <class T>
	struct LIBEUROPA_EXPORT UniqueArray final {
		UniqueArray() = default;
		explicit UniqueArray(std::size_t size) {
			Resize(size);
		}

		UniqueArray(const UniqueArray&) = delete;

		UniqueArray(UniqueArray&& move) {
			array = std::move(move.array);
			size = move.size;

			// invalidate moved-from array to default state
			move.array = nullptr;
			move.size = 0;
		}

		UniqueArray& operator=(UniqueArray&& move) {
			array = std::move(move.array);
			size = move.size;

			// invalidate moved-from array to default state
			move.array = nullptr;
			move.size = 0;
			return *this;
		}

		void Resize(std::size_t size) {
			this->array = std::make_unique<T[]>(size);
			this->size = size;
		}

		/// Clears this array.
		void Clear() {
			this->array.reset();
			this->size = 0;
		}

		T& operator[](std::size_t index) {
			return (array.get())[index];
		}

		const T& operator[](std::size_t index) const {
			return (array.get())[index];
		}

		T* Data() {
			return array.get();
		}

		const T* Data() const {
			return array.get();
		}

		std::size_t Size() const {
			return size;
		}

	   private:
		std::unique_ptr<T[]> array {};
		std::size_t size {};
	};
} // namespace europa::util