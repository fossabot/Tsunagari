/*************************************
** Tsunagari Tile Engine            **
** sounds.cpp                       **
** Copyright 2016-2020 Paul Merrill **
*************************************/

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

#include "core/sounds.h"

#include "util/int.h"
#include "util/noexcept.h"
#include "util/string-view.h"

SoundID
soundLoad(StringView path) noexcept {
    return mark;
}
void
soundsPrune(time_t latestPermissibleUse) noexcept {}

PlayingSoundID
soundPlay(SoundID id) noexcept {
    return mark;
}
void
soundRelease(SoundID id) noexcept {}

bool
playingSoundIsPlaying(PlayingSoundID id) noexcept {
    return false;
}
void
playingSoundStop(PlayingSoundID id) noexcept {}
void
playingSoundVolume(PlayingSoundID id, float volume) noexcept {}
void
playingSoundSpeed(PlayingSoundID id, float speed) noexcept {}
void
playingSoundRelease(PlayingSoundID id) noexcept {}
