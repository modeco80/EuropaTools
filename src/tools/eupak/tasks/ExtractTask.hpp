//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_EUPAK_TASKS_EXTRACTTASK_HPP
#define EUROPA_EUPAK_TASKS_EXTRACTTASK_HPP

#include <CommonDefs.hpp>

namespace eupak::tasks {

	struct ExtractTask {

		struct Arguments {
			fs::path inputPath;
			fs::path outputDirectory;
			bool verbose;
		};

		int Run(Arguments&& args);
	};


}

#endif // EUROPATOOLS_EXTRACTTASK_H
