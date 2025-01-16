//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_EUPAK_TASKS_EXTRACTTASK_HPP
#define EUROPA_EUPAK_TASKS_EXTRACTTASK_HPP

#include <CommonDefs.hpp>
#include <tasks/Task.hpp>

namespace eupak::tasks {

	struct ExtractTask : ITask {
		ExtractTask();

		void Init(argparse::ArgumentParser& parentParser) override;

		bool ShouldRun(argparse::ArgumentParser& parentParser) const override;

		int Parse() override;

		int Run() override;

	   private:
		struct Arguments {
			fs::path inputPath;
			fs::path outputDirectory;
			bool verbose;
		};

		argparse::ArgumentParser parser;
		Arguments currentArgs;
	};

} // namespace eupak::tasks

#endif // EUROPATOOLS_EXTRACTTASK_H
