//
// EuropaTools
//
// (C) 2021-2025 modeco80 <lily.modeco80@protonmail.ch>
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//

#ifndef EUROPA_IO_PAKPROGRESSREPORTSINK_H
#define EUROPA_IO_PAKPROGRESSREPORTSINK_H

#include <string>

namespace europa::io {

	/**
	 * Interface for the writer to output detailed progress information.
	 */
	struct PakProgressReportSink {

		struct PakEvent {
			enum class Type {
				FillInHeader,    	///< Filling in header
				WritingHeader,		///< Writing header

				WritingToc			///< Writing archive TOC
			};

			Type type;
		};

		struct FileEvent {
			enum class Type {
				FileBeginWrite,   ///< File has began write to package
				FileEndWrite,     ///< File writing finished
			};

			Type type;
			std::string filename;
		};

		virtual ~PakProgressReportSink() = default;

		virtual void OnEvent(const PakEvent& event) = 0;
		virtual void OnEvent(const FileEvent& event) = 0;
	};


} // namespace europa::io

#endif // EUROPA_IO_PAKPROGRESSREPORTSINK_H
