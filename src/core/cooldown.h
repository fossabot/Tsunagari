/*************************************
** Tsunagari Tile Engine            **
** cooldown.h                       **
** Copyright 2014 Michael Reiley    **
** Copyright 2014-2020 Paul Merrill **
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

#ifndef SRC_CORE_COOLDOWN_H_
#define SRC_CORE_COOLDOWN_H_

#include "util/int.h"
#include "util/noexcept.h"

/**
 * Cooldown is a timer that repeatedly expires after a specified number of
 * milliseconds.  A cooldown may hold multiple expirations, and will keep track
 * of all until told to wrap() each one.
 */
class Cooldown {
 public:
    Cooldown() noexcept;
    Cooldown(time_t duration) noexcept;

    /**
     * Sets a new duration for the Cooldown and resets any current
     * expirations.
     */
    void
    setDuration(time_t duration) noexcept;

    /**
     * Let the Cooldown know that the specified number of milliseconds have
     * passed.
     */
    void
    advance(time_t dt) noexcept;

    /**
     * Whether enough time has passed that the Cooldown has expired at
     * least once over.
     */
    bool
    hasExpired() noexcept;

    /**
     * Begin the next session, rolling over any time passed since the
     * previous expiration.
     *
     * Advances only one expiration, even if enough time has passed that
     * the cooldown has expired multiple times.  In that case, hasExpired()
     * will still return true and you may wrap again.
     */
    void
    wrapOnce() noexcept;

    void
    wrapAll() noexcept;

 private:
    time_t duration, passed;
};

#endif  // SRC_CORE_COOLDOWN_H_
