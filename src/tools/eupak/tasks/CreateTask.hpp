//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_EUPAK_TASKS_CREATETASK_HPP
#define EUROPA_EUPAK_TASKS_CREATETASK_HPP

#include <CommonDefs.hpp>

#include <europa/structs/Pak.hpp>


namespace eupak::tasks {

		struct CreateTask {
			struct Arguments {
				fs::path inputDirectory;
				fs::path outputFile;

				bool verbose;
				europa::structs::PakVersion pakVersion;
			};

			int Run(Arguments&& args);
		};

} // namespace europa

#endif // EUROPA_EUPAK_TASKS_CREATETASK_HPP
