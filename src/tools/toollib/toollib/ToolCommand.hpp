//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#pragma once
#include <argparse/argparse.hpp>
#include <memory>
#include <type_traits>

namespace tool {

	/// Base-class for all ToolLib tasks.
	struct IToolCommand {
		virtual ~IToolCommand() = default;

		/// Do any creation-time initalizatiion.
		virtual void Init(argparse::ArgumentParser& parentParser) = 0;

		/// Query if this task has been selected
		virtual bool ShouldRun(argparse::ArgumentParser& parentParser) const = 0;

		/// Parse arguments from the user
		virtual int Parse() = 0;

		/// Run the task.
		virtual int Run() = 0;
	};

	struct IToolCommandObjectCreator {
		virtual ~IToolCommandObjectCreator() = default;

		/// Creates the IToolCommand.
		virtual std::shared_ptr<IToolCommand> Create() = 0;

		// dont touch :)
		IToolCommandObjectCreator* next;
		std::string name;
	};

	/// Creates IToolCommand instances for clients.
	struct ToolCommandFactory {
		using FactoryMethod = std::shared_ptr<IToolCommand> (*)();

		/// Creates a task.
		static std::shared_ptr<IToolCommand> CreateNamed(const std::string& name);

	   private:
		template <class T>
		friend struct ToolCommandRegister;

		static void RegisterToolCommand(IToolCommandObjectCreator* pCreate);
	};

	/// Helper template to register into the [ToolCommandFactory].
	template <class T>
	struct ToolCommandRegister : IToolCommandObjectCreator {
		ToolCommandRegister(const std::string& name) {
			static_assert(std::is_base_of_v<IToolCommand, T>, "what you doing sir.");
			this->name = name;
			ToolCommandFactory::RegisterToolCommand(this);
		}

		std::shared_ptr<IToolCommand> Create() override {
			return std::make_shared<T>();
		}
	};

	/// Registers a tool command.
	/// Should be put in the .cpp implementation source file of the tool command itself.
#define TOOLLIB_REGISTER_TOOLCOMMAND(Name, TTask) \
	static ::tool::ToolCommandRegister<TTask> __register__##TTask(Name)

} // namespace tool