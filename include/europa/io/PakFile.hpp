//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKFILE_H
#define EUROPA_IO_PAKFILE_H

#include <cstdint>
#include <europa/structs/Pak.hpp>
#include <europa/util/Overloaded.hpp>
#include <filesystem>
#include <stdexcept>
#include <variant>
#include <vector>

namespace europa::io {

	struct PakReader;
	struct PakWriter;

	/// sumtype
	struct PakFileData {
		// clang-format off
		using Variant = std::variant<
			// File data
			std::vector<std::uint8_t>,

			// Path
			std::filesystem::path
		>;
		// clang-format on

		static PakFileData InitAsBuffer(std::vector<std::uint8_t>&& buffer) {
			return PakFileData {
				.variant_ = Variant(std::move(buffer))
			};
		}

		static PakFileData InitAsPath(const std::filesystem::path& path) {
			return PakFileData {
				.variant_ = Variant(path)
			};
		}

		std::uint32_t GetSize() const {
			// FIXME: make this just a overloaded lambda
			struct SizeVisitor {
				std::uint32_t& size;

				// bleh
				void operator()(const std::vector<uint8_t>& buffer) {
					size = static_cast<std::uint32_t>(buffer.size());
				}

				void operator()(const std::filesystem::path& fsPath) {
					if(!std::filesystem::exists(fsPath) && !std::filesystem::is_regular_file(fsPath))
						throw std::runtime_error("invalid path in path file");
					size = static_cast<std::uint32_t>(std::filesystem::file_size(fsPath));
				}
			};

			std::uint32_t size {};
			std::visit(SizeVisitor { size }, variant_);
			return size;
		}

		template <class T>
		const T* GetIf() const {
			return std::get_if<T>(&variant_);
		}

		template <class Visitor>
		auto Visit(Visitor&& v) const {
			return std::visit(v, variant_);
		}

		// private:
		PakFileData::Variant variant_;
	};

	/// Repressents a package file. Can either hold a memory buffer of contents
	/// or a filesystem path (for creating packages).
	struct PakFile {
		using DataType = PakFileData;

		template <class T>
		void InitWithExistingTocEntry(const T& value) {
			toc = value;
		}

		void InitAs(structs::PakVersion version) {
			switch(version) {
				case structs::PakVersion::Ver3:
					toc = structs::PakHeader_V3::TocEntry {};
					break;
				case structs::PakVersion::Ver4:
					toc = structs::PakHeader_V4::TocEntry {};
					break;
				case structs::PakVersion::Ver5:
					toc = structs::PakHeader_V5::TocEntry {};
					break;
				default:
					throw std::invalid_argument("Invalid PAK version to initalize TOC entry");
					break;
			}
		}

		bool HasData() const {
			return fileData.has_value();
		}

		/**
		 * Get the file data.
		 */
		[[nodiscard]] const DataType& GetData() const {
			if(!fileData.has_value())
				throw std::runtime_error("no file data to get!");
			return fileData.value();
		}

		/// Sets data.
		void SetData(DataType&& data) {
			this->fileData = std::move(data);

			// Update the TOC size.
			std::visit([&](auto& entry) {
				entry.size = this->fileData.value().GetSize();
			},
					   toc);
		}

		/// Purge read file data.
		void PurgeData() {
			this->fileData = std::nullopt;
		}

		/**
		 * Get the TOC entry responsible.
		 */
		template <class T>
		[[nodiscard]] const T& GetTOCEntry() const {
			return std::get<T>(toc);
		}

		std::uint32_t GetCreationUnixTime() const {
			std::uint32_t time {};

			std::visit([&](auto& entry) {
				time = entry.creationUnixTime;
			},
					   toc);

			return time;
		}

		std::uint32_t GetOffset() const {
			std::uint32_t size {};

			std::visit([&](auto& entry) {
				size = entry.offset;
			},
					   toc);

			return size;
		}

		std::uint32_t GetSize() const {
			std::uint32_t size {};

			std::visit([&](auto& entry) {
				size = entry.size;
			},
					   toc);

			return size;
		}

		template <class Visitor>
		auto VisitTocEntry(Visitor&& cb) {
			return std::visit(cb, toc);
		}

	   private:
		friend PakReader;
		friend PakWriter;

		std::optional<PakFileData> fileData;
		structs::PakTocEntryVariant toc;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKFILE_H
