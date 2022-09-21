//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#include <europa/io/PakFile.hpp>

namespace europa::io {

	const PakFile::DataType& PakFile::GetData() const {
		return data;
	}

	const structs::PakTocEntry& PakFile::GetTOCEntry() const {
		return tocData;
	}

	structs::PakTocEntry& PakFile::GetTOCEntry() {
		return tocData;
	}

	void PakFile::SetData(PakFile::DataType&& newData) {
		data = std::move(newData);
	}

	void PakFile::FillTOCEntry() {
		tocData.size = static_cast<std::uint32_t>(data.size());
	}

} // namespace europa::io