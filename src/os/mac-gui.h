/****************************************
** Tsunagari Tile Engine               **
** os/mac.h                            **
** Copyright 2013      Michael Reiley  **
** Copyright 2013-2016 Paul Merrill    **
****************************************/

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

#ifdef __APPLE__

#    ifndef SRC_OS_MAC_H_
#        define SRC_OS_MAC_H_

#include "util/string-view.h"

/**
 * Sets the current working directory to the "Resources" folder in the .app
 * package.
 */
void macSetWorkingDirectory() noexcept;

/**
 * Displays a message box window.
 */
void macMessageBox(StringView title, StringView msg) noexcept;

#    endif  // SRC_OS_MAC_H_

#endif  // __APPLE__
