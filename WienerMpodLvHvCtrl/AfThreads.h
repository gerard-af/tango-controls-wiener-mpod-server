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

#ifndef __AfThreads__h__
#define __AfThreads__h__

#ifdef AF_WP_QT

#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

//#define AF_WP_DEBUG_QT 1

class AfThreadsMutex : public QMutex {
public:
    AfThreadsMutex() {}
};

class AfThreadsMutexLocker : public QMutexLocker {
public:
    AfThreadsMutexLocker(QMutex &mutex) : QMutexLocker(&mutex) {}
};

class AfThread {

public:

    AfThread() {
    }
    ~AfThread() {
    }

    bool createThread() {
        return false;
    }
    void joinThread() {}
    void joinThread(size_t wait_delay_ms) {}
    void interruptThread(bool flag_force = false) {}
    void joinOrInterruptThread(size_t wait_delay_ms) {}

    virtual void executeThread() = 0;

private:

    void initThread() {
    }

};


#elif defined WIN32

class AfThreadsMutex {

    void *mMutexHandle;

public:

        AfThreadsMutex();
        ~AfThreadsMutex();

    void lock();
    void unlock();

};

class AfThreadsMutexLocker {

        AfThreadsMutex &m_mutex;

public:
        AfThreadsMutexLocker(AfThreadsMutex &mutex) : m_mutex(mutex) {
        m_mutex.lock();
    }
        ~AfThreadsMutexLocker() {
        m_mutex.unlock();
    }
};

class AfThread {

    void *mThreadHandle;

public:

        AfThread();
        ~AfThread();
    bool createThread();
    void joinThread();
    virtual void executeThread() = 0;

private:

    void initThread();

};

#else

#include <pthread.h>
#include <exception>

class AfThreadsMutex {

    pthread_mutex_t m_mutex;

public:

    AfThreadsMutex() {
        pthread_mutex_init(&m_mutex, 0);
    }
    ~AfThreadsMutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

};

class AfThreadsMutexLocker {

    AfThreadsMutex &m_mutex;

public:
    AfThreadsMutexLocker(AfThreadsMutex &mutex) : m_mutex(mutex) {
        m_mutex.lock();
    }
    ~AfThreadsMutexLocker() {
        m_mutex.unlock();
    }
};

void *af_wp_thread_start_routine(void *data);

class AfThread {

    pthread_t mThread;

public:

    AfThread() {
        mThread = 0;
        initThread();
    }
    ~AfThread() {
    }

    bool createThread() {
        joinThread();
        int success = pthread_create(&mThread, 0, &af_wp_thread_start_routine, (void *)(this));
        if ( success == 0 ) {
            return true;
        }
        return false;
    }
    void joinThread() {
        void *status;
        pthread_join(mThread, &status);
    }

    virtual void executeThread() = 0;

private:

    void initThread() {
    }

};

inline void *af_wp_thread_start_routine(void *data) {
    AfThread *thread = (AfThread*)(data);
    try {
        thread->executeThread();
    }
    catch ( std::exception &ex ) {
    }
    pthread_exit(0);
}

#endif



#endif

