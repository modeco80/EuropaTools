//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKFILE_H
#define EUROPA_IO_PAKFILE_H

#include <cstdint>
#include <europa/structs/Pak.hpp>
#include <vector>

namespace europa::io {

	struct PakReader;
	struct PakWriter;

	/// Repressents a package file.
	/// FIXME: Maybe make this not hold a buffer at some point,
	/// or a sumtype which can contain either buffer OR path to os file
	/// (which we can then efficiently tee into)
	struct PakFile {
		using DataType = std::vector<std::uint8_t>;

		template<class T>
		void InitAs(const T& value) {
			toc = value;
		}

		void InitAs(structs::PakVersion version) {
			switch(version) {
				case structs::PakVersion::Ver3:
					toc = structs::PakHeader_V3::TocEntry{};
					break;
				case structs::PakVersion::Ver4:
					toc = structs::PakHeader_V4::TocEntry{};
					break;
				case structs::PakVersion::Ver5:
					toc = structs::PakHeader_V5::TocEntry{};
					break;
			}
		}

		/**
		 * Get the file data.
		 */
		[[nodiscard]] const DataType& GetData() const {
			return data;
		}

		/**
		 * Get the TOC entry responsible.
		 */
		template<class T>
		[[nodiscard]] const T& GetTOCEntry() const {
			return std::get<T>(toc);
		}

		void SetData(DataType&& data) {
			this->data = std::move(data);
			
			// Update the TOC size.
			std::visit([&](auto& entry) {
				entry.size = this->data.size();
			}, toc);
		}

		std::uint32_t GetCreationUnixTime() const {
			std::uint32_t time{};

			std::visit([&](auto& entry) {
				time = entry.creationUnixTime;
			}, toc);

			return time;
		}

		std::uint32_t GetOffset() const {
			std::uint32_t size{};

			std::visit([&](auto& entry) {
				size = entry.offset;
			}, toc);

			return size;
		}

		std::uint32_t GetSize() const {
			std::uint32_t size{};

			std::visit([&](auto& entry) {
				size = entry.size;
			}, toc);

			return size;
		}

		void FillTOCEntry() {
			std::visit([&](auto& entry) {
				entry.size = static_cast<std::uint32_t>(data.size());
			}, toc);
		}

		template<class Cb>
		void Visit(const Cb& cb) {
			std::visit(cb, toc);
		}

	   private:
		friend PakReader;
		friend PakWriter;

		std::vector<std::uint8_t> data;
		structs::PakTocEntryVariant toc;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKFILE_H
