/*

    Copyright (c) 2018 - 2019 Analog Flavor EURL

    Contact us for support:
        http://www.analogflavor.com

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Analog Flavor EURL nor 
      the names of its contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------

*/

#include "AfThreads.h"

#ifdef AF_WP_QT

#elif defined WIN32

#include <windows.h>
#include <exception>

AfThreadsMutex::AfThreadsMutex() {
    mMutexHandle = ::CreateMutex(0, FALSE, 0);
}

AfThreadsMutex::~AfThreadsMutex() {
    if (mMutexHandle) {
        ::CloseHandle(mMutexHandle);
        mMutexHandle = 0;
    }
}

void AfThreadsMutex::lock() {
    ::WaitForSingleObject(mMutexHandle, INFINITE);
}

void AfThreadsMutex::unlock() {
    ::ReleaseMutex(mMutexHandle);
}

DWORD WINAPI af_wp_thread_start_routine(void *data);

AfThread::AfThread() {
    initThread();
}
AfThread::~AfThread() {
    // interruptThread(true);
    if (mThreadHandle) {
        if (::CloseHandle(mThreadHandle)) {
            mThreadHandle = 0;
        }
    }
}

bool AfThread::createThread() {
    joinThread();
    mThreadHandle = ::CreateThread(0, 0, &af_wp_thread_start_routine, (void *)(this), 0, 0);
    if (mThreadHandle) {
        return true;
    }
    return false;
}
void AfThread::joinThread() {
    if (mThreadHandle) {
        ::WaitForSingleObject(mThreadHandle, INFINITE);
    }
}

void AfThread::initThread() {
    mThreadHandle = 0;
}

inline DWORD WINAPI af_wp_thread_start_routine(void *data) {
        AfThread *thread = (AfThread*)(data);
    try {
        thread->executeThread();
    }
    catch (std::exception &ex) {
    }
    return 0;
}

#endif

