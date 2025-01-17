/***************************************
** Tsunagari Tile Engine              **
** log.h                              **
** Copyright 2011-2013 Michael Reiley **
** Copyright 2011-2020 Paul Merrill   **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#ifndef SRC_CORE_LOG_H_
#define SRC_CORE_LOG_H_

#include "util/string-view.h"

enum LogVerbosity {
    QUIET = 1,  // Display fatals.
    NORMAL,     // Display fatals and errors.
    VERBOSE     // Display fatals, errors and info.
};

// Initialize the clock for log timestamps.
bool
logInit() noexcept;

// Set the logging verbosity. Some log messages may be suppressed depending
// on this setting.
void
logSetVerbosity(LogVerbosity mode) noexcept;

// Log an info message to the console if verbosity is V_VERBOSE.
void
logInfo(StringView domain, StringView msg) noexcept;

// Log an error message to the console if verbosity is V_VERBOSE or
// V_NORMAL.
void
logErr(StringView domain, StringView msg) noexcept;

// Log a fatal error message to the console.
void
logFatal(StringView domain, StringView msg) noexcept;

// Used by main() to report the verbosity setting on engine startup.
void
logReportVerbosityOnStartup() noexcept;

#endif  // SRC_CORE_LOG_H_
