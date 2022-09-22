//
// EuropaTools
//
// (C) 2021-2022 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef EUROPA_EUPAK_TASKS_INFOTASK_HPP
#define EUROPA_EUPAK_TASKS_INFOTASK_HPP

#include <CommonDefs.hpp>

namespace eupak::tasks {

		struct InfoTask {

			struct Arguments {
				fs::path inputPath;
				bool verbose;
			};

			int Run(Arguments&& args);
		};

}

#endif // EUROPA_EUPAK_TASKS_INFOTASK_HPP
