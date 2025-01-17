/*************************************
** Tsunagari Tile Engine            **
** unix-condition-variable.h        **
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

#ifndef SRC_OS_UNIX_CONDITION_VARIBLE_H_
#define SRC_OS_UNIX_CONDITION_VARIBLE_H_

#include "os/c.h"
#include "os/mutex.h"
#include "util/assert.h"

class ConditionVariable {
 public:
    inline ConditionVariable() : cv(PTHREAD_COND_INITIALIZER) {}

    inline ~ConditionVariable() noexcept {
        int err = pthread_cond_destroy(&cv);
        (void)err;
        assert_(err == 0);
    }

    ConditionVariable(const ConditionVariable&) = delete;
    ConditionVariable&
    operator=(const ConditionVariable&) = delete;

    inline void
    notifyOne() noexcept {
        int err = pthread_cond_signal(&cv);
        (void)err;
        assert_(err == 0);
    }

    inline void
    notifyAll() noexcept {
        int err = pthread_cond_broadcast(&cv);
        (void)err;
        assert_(err == 0);
    }

    inline void
    wait(LockGuard& lock) noexcept {
        int err = pthread_cond_wait(&cv, &lock.m.m);
        (void)err;
        assert_(err == 0);
    }

    pthread_cond_t cv;
};

#endif  // SRC_OS_UNIX_CONDITION_VARIBLE_H_
