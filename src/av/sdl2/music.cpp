/********************************
** Tsunagari Tile Engine       **
** music.cpp                   **
** Copyright 2016 Paul Merrill **
********************************/

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

#include "av/sdl2/music.h"

static SDL2Music globalMusicWorker;

MusicWorker& MusicWorker::instance() {
    return globalMusicWorker;
}

void SDL2Music::setIntro(const std::string& filename) {}
void SDL2Music::setLoop(const std::string& filename) {}

void SDL2Music::stop() {}

bool SDL2Music::playing() {}
void SDL2Music::pause() {}
void SDL2Music::resume() {}

void SDL2Music::setVolume(double volume) {}

void SDL2Music::tick() {}

void SDL2Music::garbageCollect() {}