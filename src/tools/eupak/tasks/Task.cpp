//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include "Task.hpp"

#include <stdexcept>

namespace eupak::tasks {

	std::shared_ptr<ITask> TaskFactory::CreateNamed(const std::string& name, argparse::ArgumentParser& parentParser) {
		const auto& m = factoryMap();
		if(m.contains(name)) {
			auto task = m.at(name)();
			task->Init(parentParser);
			return task;
		} else
			throw std::runtime_error("Invalid task factory creation request");
	}

} // namespace eupak::tasks