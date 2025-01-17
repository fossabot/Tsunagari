/*************************************
** Tsunagari Tile Engine            **
** windows-mutex.h                  **
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

#ifndef SRC_OS_WINDOWS_MUTEX_H_
#define SRC_OS_WINDOWS_MUTEX_H_

#include "os/c.h"
#include "util/constexpr.h"
#include "util/noexcept.h"

extern "C" {
typedef struct {
    PVOID Ptr;
} SRWLOCK, *PSRWLOCK;

#define SRWLOCK_INIT \
    { 0 }

WINBASEAPI VOID WINAPI
AcquireSRWLockExclusive(PSRWLOCK SRWLock) noexcept;
WINBASEAPI BOOLEAN WINAPI
TryAcquireSRWLockExclusive(PSRWLOCK SRWLock) noexcept;
WINBASEAPI VOID WINAPI
ReleaseSRWLockExclusive(PSRWLOCK SRWLock) noexcept;
}

class Mutex {
 public:
    constexpr Mutex() noexcept : m(SRWLOCK_INIT) {}

    inline void
    lock() noexcept {
        AcquireSRWLockExclusive(&m);
    }
    inline bool
    tryLock() noexcept {
        return TryAcquireSRWLockExclusive(&m) != 0;
    }
    inline void
    unlock() noexcept {
        ReleaseSRWLockExclusive(&m);
    }

    SRWLOCK m;

 private:
    Mutex(const Mutex&) = delete;
    Mutex&
    operator=(const Mutex&) = delete;
};

#endif  // SRC_OS_WINDOWS_MUTEX_H_
