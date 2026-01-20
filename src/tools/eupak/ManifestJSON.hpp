//
// EuropaTools
//
// (C) 2021-2026 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

//! Manifest data structures.
#pragma once
#include <daw/json/daw_json_link.h>

#include <cstdint>
#include <europa/io/pak/Writer.hpp>
#include <europa/structs/Pak.hpp>
#include <string>

#include "CommonDefs.hpp"

namespace eupak {

    struct ManifestFile {
        std::string path;
        std::string sourcePath;
        std::optional<std::uint32_t> creationTime;
    };

    struct ManifestRoot {
        estructs::PakVersion version;
        std::optional<eio::pak::Writer::SectorAlignment> alignment;
        std::optional<std::uint32_t> creationTime;

        std::vector<ManifestFile> files;
        std::vector<std::string> tocOrder;
    };

} // namespace eupak

namespace daw::json {
    template <>
    struct json_data_contract<eupak::ManifestFile> {
        using type = json_member_list<
        json_string<"path">,
        json_string<"sourcePath">,
        json_number_null<"creationTime", std::optional<std::uint32_t>>>;

        static inline auto to_json_data(const eupak::ManifestFile& file) {
            return std::forward_as_tuple(file.path, file.sourcePath, file.creationTime);
        }
    };

    template <>
    struct json_data_contract<eupak::ManifestRoot> {
        using type = json_member_list<
        json_number<"version", europa::structs::PakVersion>,
        json_number_null<"alignment", std::optional<europa::io::pak::Writer::SectorAlignment>>,
        json_number_null<"creationTime", std::optional<std::uint32_t>>,
        json_array<"files", eupak::ManifestFile>,
        json_array<"tocOrder", std::string>>;

        static inline auto to_json_data(const eupak::ManifestRoot& root) {
            return std::forward_as_tuple(root.version, root.alignment, root.creationTime, root.files, root.tocOrder);
        }
    };
} // namespace daw::json
