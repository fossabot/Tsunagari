/*************************************
** Tsunagari Tile Engine            **
** unix-mutex.h                     **
** Copyright 2019-2020 Paul Merrill **
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

#ifndef SRC_OS_UNIX_MUTEX_H_
#define SRC_OS_UNIX_MUTEX_H_

#include "os/c.h"
#include "util/assert.h"

class Mutex {
 public:
    inline Mutex() noexcept : m(PTHREAD_MUTEX_INITIALIZER) {}

    inline ~Mutex() noexcept {
        int err = pthread_mutex_destroy(&m);
        (void)err;
        assert_(err == 0);
    }

    inline void
    lock() noexcept {
        int err = pthread_mutex_lock(&m);
        (void)err;
        assert_(err == 0);
    };

    inline void
    unlock() noexcept {
        int err = pthread_mutex_unlock(&m);
        (void)err;
        assert_(err == 0);
    }

    Mutex(const Mutex&) = delete;
    Mutex&
    operator=(const Mutex&) = delete;

    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
};

#endif  // SRC_OS_UNIX_MUTEX_H_
