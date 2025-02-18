//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: MIT
//

#ifndef EUROPA_IO_PAKPROGRESSREPORTSINK_H
#define EUROPA_IO_PAKPROGRESSREPORTSINK_H

#include <string>

namespace europa::io::pak {

	/// Interface for [PakWriter] to output detailed progress information.
	struct WriterProgressReportSink {
		struct PakEvent {
			enum class EventCode {
				FillInHeader,  /// Filling in header.
				WritingHeader, /// Writing header.
				WritingToc	   /// Writing archive TOC.
			};

			EventCode eventCode;
		};

		struct FileEvent {
			enum class EventCode {
				FileWriteBegin, /// File has began write to package
				FileWriteEnd,	/// File has been written to package
			};

			EventCode eventCode;
			const std::string& targetFileName;
		};

		virtual ~WriterProgressReportSink() = default;

		virtual void OnEvent(const PakEvent& event) = 0;
		virtual void OnEvent(const FileEvent& event) = 0;
	};

} // namespace europa::io

#endif // EUROPA_IO_PAKPROGRESSREPORTSINK_H
