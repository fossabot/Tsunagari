/***************************************
** Tsunagari Tile Engine              **
** client-conf.h                      **
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

#ifndef SRC_CORE_CLIENT_CONF_H_
#define SRC_CORE_CLIENT_CONF_H_

#include "core/log.h"
#include "core/vec.h"
#include "util/int.h"
#include "util/string-view.h"

//! Engine-wide user-confurable values.

//! Game Movement Mode
enum MoveMode { TURN, TILE, NOTILE };

extern LogVerbosity confVerbosity;
extern MoveMode confMoveMode;
extern ivec2 confWindowSize;
extern bool confFullscreen;
extern int confMusicVolume;
extern int confSoundVolume;
extern time_t confCacheTTL;
extern int confPersistInit;
extern int confPersistCons;

bool
confParse(StringView filename) noexcept;

#endif  // SRC_CORE_CLIENT_CONF_H_
