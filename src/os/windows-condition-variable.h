/*********************************
** Tsunagari Tile Engine        **
** windows-condition-variable.h **
** Copyright 2019 Paul Merrill  **
*********************************/

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

#ifndef SRC_OS_WINDOWS_CONDITION_VARIBLE_H_
#define SRC_OS_WINDOWS_CONDITION_VARIBLE_H_

#include "os/c.h"
#include "os/mutex.h"
#include "util/assert.h"
#include "util/noexcept.h"

extern "C" {
typedef struct {
    PVOID Ptr;
} CONDITION_VARIABLE, *PCONDITION_VARIABLE;

#define CONDITION_VARIABLE_INIT \
    { 0 }

WINBASEAPI VOID WINAPI
WakeConditionVariable(PCONDITION_VARIABLE ConditionVariable) noexcept;

WINBASEAPI VOID WINAPI
WakeAllConditionVariable(PCONDITION_VARIABLE ConditionVariable) noexcept;

WINBASEAPI BOOL WINAPI
SleepConditionVariableSRW(PCONDITION_VARIABLE ConditionVariable,
                          PSRWLOCK SRWLock,
                          DWORD dwMilliseconds,
                          ULONG Flags) noexcept;

#define INFINITE 0xFFFFFFFF  // Infinite timeout.
}

class ConditionVariable {
 public:
    inline ConditionVariable() noexcept : cv(CONDITION_VARIABLE_INIT) {}

    ConditionVariable(const ConditionVariable&) = delete;
    ConditionVariable&
    operator=(const ConditionVariable&) = delete;

    inline void
    notifyOne() noexcept {
        WakeConditionVariable(&cv);
    }
    inline void
    notifyAll() noexcept {
        WakeAllConditionVariable(&cv);
    }

    inline void
    wait(LockGuard& lock) noexcept {
        BOOL ok = SleepConditionVariableSRW(&cv, &lock.m.m, INFINITE, 0);
        (void)ok;
        assert_(ok);  // GetLastError();
    }

    CONDITION_VARIABLE cv;
};

#endif  // SRC_OS_WINDOWS_CONDITION_VARIBLE_H_
