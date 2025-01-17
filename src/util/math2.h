/***************************************
** Tsunagari Tile Engine              **
** math2.h                            **
** Copyright 2014      Michael Reiley **
** Copyright 2014-2020 Paul Merrill   **
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

// pow2()
//
// Original source downloaded from:
//   https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2

/*
 * Individually, the code snippets here are in the public domain (unless
 * otherwise noted)--feel free to use them however you please. The aggregate
 * collection and descriptions are (c) 1997-2005 Sean Eron Anderson. The code
 * and descriptions are distributed in the hope that they will be useful, but
 * WITHOUT ANY WARRANTY and without even the implied warranty of
 * merchantability or fitness for a particular purpose.
 */

#ifndef SRC_UTIL_MATH2_H_
#define SRC_UTIL_MATH2_H_

#include "util/int.h"
#include "util/noexcept.h"

template<typename T>
static T
max(T a, T b) noexcept {
    return a < b ? b : a;
}

template<typename T>
static T
min(T a, T b) noexcept {
    return a < b ? a : b;
}

template<typename T>
static T
bound(T value, T min, T max) noexcept {
    if (value < min) {
        return min;
    }
    else if (value > max) {
        return max;
    }
    else {
        return value;
    }
}

template<typename T>
static T
wrap(T min, T value, T max) noexcept {
    while (value < min) {
        value += max;
    }
    return value % max;
}

// Rounds a number up to the nearest power of 2, or to 0, whichever is lower.
static uint32_t
pow2(uint32_t i) noexcept {
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    i++;
    return i;
}
static uint64_t
pow2(uint64_t i) noexcept {
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    i |= i >> 32;
    i++;
    return i;
}

// FIXME: Why does Apple need this?
#if defined(__APPLE__)
static size_t
pow2(size_t i) noexcept {
    return pow2(static_cast<uint64_t>(i));
}
#endif

#endif  // SRC_UTIL_MATH2_H_
