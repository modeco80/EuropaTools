//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_EUPAK_TASKS_CREATETASK_HPP
#define EUROPA_EUPAK_TASKS_CREATETASK_HPP

#include <CommonDefs.hpp>
#include <europa/structs/Pak.hpp>
#include <tasks/Task.hpp>

#include "argparse/argparse.hpp"

namespace eupak::tasks {

	struct CreateTask : ITask {
		/// Arguments.
		/// This is no longer directly used by clients
		/// and should later be factored out of surface.
		struct Arguments {
			fs::path inputDirectory;
			fs::path outputFile;

			bool verbose;
			europa::structs::PakVersion pakVersion;
			bool sectorAligned;
		};

		CreateTask();

		void Init(argparse::ArgumentParser& parentParser) override;

		bool ShouldRun(argparse::ArgumentParser& parentParser) const override;

		int Parse() override;

		int Run() override;

	   private:
		argparse::ArgumentParser parser;
		Arguments currentArgs;
	};

} // namespace eupak::tasks

#endif // EUROPA_EUPAK_TASKS_CREATETASK_HPP
