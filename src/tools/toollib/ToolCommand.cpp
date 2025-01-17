//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#include <format>
#include <stdexcept>
#include <toollib/ToolCommand.hpp>

namespace tool {

	IToolCommandObjectCreator* pToolCommandListHead = nullptr;

	void ToolCommandFactory::RegisterToolCommand(IToolCommandObjectCreator* pCreate) {
		if(pToolCommandListHead != nullptr) {
			// Find the first linked creator with a
			// nullptr next link (the end of the list).
			//
			// Once we do, attach the command creator
			auto* p = pToolCommandListHead;
			while(p->next != nullptr) {
				p = p->next;
			}

			p->next = pCreate;
		} else {
			pToolCommandListHead = pCreate;
		}
	}

	IToolCommandObjectCreator* FindCreator(const std::string& name) {
		if(!pToolCommandListHead)
			return nullptr;

		auto* p = pToolCommandListHead;
		while(p != nullptr) {
			if(p->name == name)
				return p;

			p = p->next;
		}

		return nullptr;
	}

	std::shared_ptr<IToolCommand> ToolCommandFactory::CreateNamed(const std::string& name) {
		if(auto pCreator = FindCreator(name); pCreator) {
			auto cmd = pCreator->Create();
			return cmd;
		} else {
			throw std::runtime_error(std::format("Invalid tool command \"{}\"", name));
		}
	}

} // namespace tool