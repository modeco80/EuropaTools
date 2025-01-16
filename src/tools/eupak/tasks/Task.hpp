//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once
#include <CommonDefs.hpp>
#include <type_traits>
#include <unordered_map>

#include "argparse/argparse.hpp"

namespace eupak::tasks {

	/// Base-class for all eupak tasks.
	struct ITask {
		virtual ~ITask() = default;

		/// Do any creation-time initalizatiion.
		virtual void Init(argparse::ArgumentParser& parentParser) = 0;

		/// Query if this task has been selected
		virtual bool ShouldRun(argparse::ArgumentParser& parentParser) const = 0;

		/// Parse arguments from the user
		virtual int Parse() = 0;

		/// Run the task.
		virtual int Run() = 0;
	};

	/// Creates ITask instances for clients.
	struct TaskFactory {
		using FactoryMethod = std::shared_ptr<ITask> (*)();

		/// Creates a task.
		static std::shared_ptr<ITask> CreateNamed(const std::string& name, argparse::ArgumentParser& parentParser);

	   private:
		template <class T>
		friend struct TaskFactoryRegister;

		static auto& factoryMap() {
			static std::unordered_map<std::string, FactoryMethod> m;
			return m;
		}
	};

    /// Helper template to register into the [TaskFactory].
	template <class T>
	struct TaskFactoryRegister {
		TaskFactoryRegister(const std::string& name) {
			static_assert(std::is_base_of_v<ITask, T>, "cannot register a type which does not derive from ITask");
			TaskFactory::factoryMap().insert({ name,
											   []() -> std::shared_ptr<ITask> {
												   return std::make_shared<T>();
											   } });
		}
	};

    /// Registers a task. Should be put in the .cpp implementation source file of the 
    /// task object itself.
#define EUPAK_REGISTER_TASK(Name, TTask) \
	static ::eupak::tasks::TaskFactoryRegister<TTask> __register__##TTask(Name)

} // namespace eupak::tasks