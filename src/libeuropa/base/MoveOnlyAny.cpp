#include <europa/base/MoveOnlyAny.hpp>

namespace europa::base {
	MoveOnlyAny::~MoveOnlyAny() {
		DestroyValue();
	}

	bool MoveOnlyAny::HasValue() const {
		if(control == nullptr)
			return false;
		return pAnyPtr != nullptr;
	}

	void MoveOnlyAny::CreateBase(ControlRoutine routine) {
		// Destroy an existing value to avoid memory leaks.
		if(HasValue()) {
			DestroyValue();
		}

		this->control = routine;
		control(this, ControlOp::Create, nullptr);
	}

	void MoveOnlyAny::CopyBase(ControlRoutine routine, void* pCopyObject) {
		this->control = routine;
		CopyFromSourceArgs arg { .pSourceObject = pCopyObject };
		control(this, ControlOp::CopyFromSource, &arg);
	}

	void MoveOnlyAny::DestroyValue() {
		if(HasValue()) {
			control(this, ControlOp::Delete, nullptr);
			this->control = nullptr;
		}
	}
} // namespace europa::base