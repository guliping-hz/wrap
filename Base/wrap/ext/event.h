﻿#ifndef EVENT__WRAPPER__H__
#define EVENT__WRAPPER__H__

#ifdef WIN32

#include <windows.h>
#include "../typedefs.h"

#else

#include <pthread.h>
#include <time.h>
#include "thread.h"

#endif // WIN32

enum EventTypeWrapper {
	kEventSignaled = 1,
	kEventError = 2,
	kEventTimeout = 3
};

#define UTIL_EVENT_10_SEC   10000
#define UTIL_EVENT_INFINITE 0xffffffff

class EventWrapper {
public:
	// Factory method. Constructor disabled.
	static EventWrapper* Create();
	virtual ~EventWrapper() {}

	// Releases threads who are calling Wait() and has started waiting. Please
	// note that a thread calling Wait() will not start waiting immediately.
	// assumptions to the contrary is a very common source of issues in
	// multithreaded programming.
	// Set is sticky in the sense that it will release at least one thread
	// either immediately or some time in the future.
	virtual bool Set() = 0;

	// Prevents future Wait() calls from finishing without a new Set() call.
	virtual bool Reset() = 0;

	// Puts the calling thread into a wait state. The thread may be released
	// by a Set() call depending on if other threads are waiting and if so on
	// timing. The thread that was released will call Reset() before leaving
	// preventing more threads from being released. If multiple threads
	// are waiting for the same Set(), only one (random) thread is guaranteed to
	// be released. It is possible that multiple (random) threads are released
	// Depending on timing.
	virtual EventTypeWrapper Wait(unsigned long max_time) = 0;

	// Starts a timer that will call a non-sticky version of Set() either once
	// or periodically. If the timer is periodic it ensures that there is no
	// drift over time relative to the system clock.
	virtual bool StartTimer(bool periodic, unsigned long time) = 0;

	virtual bool StopTimer() = 0;

};

#ifdef WIN32

class EventWindows : public EventWrapper {
public:
	EventWindows();
	virtual ~EventWindows();

	//miro second
	virtual EventTypeWrapper Wait(unsigned long max_time);
	virtual bool Set();
	virtual bool Reset();

	virtual bool StartTimer(bool periodic, unsigned long time);
	virtual bool StopTimer();

private:
	HANDLE  event_;
	uint32_t timerID_;
};

#else

enum eState {
	kUp = 1,
	kDown = 2
};

class EventPosix : public EventWrapper {
public:
	static EventWrapper* Create();

	virtual ~EventPosix();

	virtual EventTypeWrapper Wait(unsigned long max_time);
	virtual bool Set();
	virtual bool Reset();

	virtual bool StartTimer(bool periodic, unsigned long time);
	virtual bool StopTimer();

private:
	EventPosix();
	int Construct();

	static bool Run(ThreadObj obj);
	bool Process();
	EventTypeWrapper Wait(timespec& wake_at);

private:
	pthread_cond_t  cond_;
	pthread_mutex_t mutex_;

	ThreadWrapper* timer_thread_;
	EventPosix*    timer_event_;
	timespec       created_at_;

	bool          periodic_;
	unsigned long time_;  // In ms
	unsigned long count_;
	eState         state_;

	bool			isInWait_;
};

#endif // WIN32

#endif  // EVENT__WRAPPER__H__
