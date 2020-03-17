/*
Copyright 2007-2014. Algoryx Simulation AB.

All AgX source code, intellectual property, documentation, sample code,
tutorials, scene files and technical white papers, are copyrighted, proprietary
and confidential material of Algoryx Simulation AB. You may not download, read,
store, distribute, publish, copy or otherwise disseminate, use or expose this
material without having a written signed agreement with Algoryx Simulation AB.

Algoryx Simulation AB disclaims all responsibilities for loss or damage caused
from using this software, unless otherwise stated in written agreements with
Algoryx Simulation AB.
*/

#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

#if defined(_MSC_VER) 


#include <intrin.h> 
#pragma intrinsic(__rdtsc) 

#endif



#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

// Still supporting compilers without full support for C++11, so cannot use chrono yet.


#ifdef _WIN32

#else
#include <unistd.h>
#endif

namespace vr {

	typedef double Real64;
	#ifdef _WIN32
	typedef unsigned __int64 UInt64;
	#else
	typedef uint64_t UInt64;
	typedef unsigned int UInt;

	#endif
	/**
	The Timer cla
	ss permits timing execution speed with the same
	refinement as the built in hardware clock of the CPU.

	Profiling tools typically have sampling rates which are too coarse for
	detailed timing analysis. The Timer class uses the hardware clock to
	record beginning and end times of given code sections.
	(For Windows, QueryPerformanceFrequency/QueryPerformanceTimer is used if VR_USE_HPET is set).
	*/
	class Timer
	{
	public:

		/**
		Creates a timer.
		\param startImmediately Should the timer start immediately or start in stopped mode
		(now, but also after consequent calls to reset())?
		*/
		Timer(bool startImmediately = false);

		/**
		Starts the timer.
		Calling start on a already started timer will have no effect.
		*/
		void start();

		/**
		Stops the timer.
		Calling stop on a already stopped timer will have no effect.
		Upon stopping, the amount of cycles and time ran will be updated.
		*/
		void stop();

		/**
		Clear all data.
		*/
		void reset(bool startAfterReset = false);

		/**
		Report total elapsed time since start in milliseconds, excluding
		intervals when the timer was suspended.
		*/
		Real64 getTime() const;

		/// Report execution time in CPU cycles. 
		UInt64 getNumTicks() const;

		/// \return the start tick.
		UInt64 getStartTick() const;

		/// \return true if running.
		bool isRunning() const;

		/// \return raw value of the TSC register (in Windows: QueryPerformanceTimer if VR_USE_HPET is set). 
		static UInt64 getCurrentTick();

		/// \return time in ms relative to a start tick
		static Real64 getTime(UInt64 startTick);

		/// \return the given number of ticks in ms.
		static Real64 convertToMs(UInt64 ticks);

		/// For internal use only.
		static void initClockSpeed();

	private:
		static double clockSpeed();
#if defined(_MSC_VER)
		/// Report execution time in CPU cycles. 
		static UInt64 getCurrentTickWindows();
#endif

	private:


		UInt64 m_start;
		UInt64 m_total;


		bool m_running;

		static double s_clockSpeed;
	};


	/* Implementation */
	inline Timer::Timer(bool startImmediately)
	{
		reset();
		if (startImmediately)
			this->start();
	}


	inline void Timer::start()
	{
		if (m_running)
			return;

		m_running = true;

#if AGX_USE_CPP11_DISABLED
		m_start = std::high_resolution_clock::now();
#else
		m_start = getCurrentTick();
#endif
	}


	inline void Timer::stop()
	{
		if (m_running) {
			m_running = false;


			UInt64 now = getCurrentTick();
			m_total += (now - m_start);

		}
	}


	inline void Timer::reset(bool startAfterReset)
	{
		m_start = 0L;
		m_total = 0L;

		m_running = false;
		if (startAfterReset)
			this->start();
	}


	inline UInt64 Timer::getNumTicks() const
	{
		return m_total;
	}


	inline UInt64 Timer::getStartTick() const
	{
		return m_start;
	}


	inline bool Timer::isRunning() const
	{
		return m_running;
	}


	inline Real64 Timer::getTime() const
	{

		return m_running ? (clockSpeed() * (Real64)(getCurrentTick() - m_start + m_total)) : clockSpeed() * (Real64)m_total;

	}


	inline double Timer::clockSpeed()
	{
		return s_clockSpeed;
	}


	inline UInt64 Timer::getCurrentTick()
	{
#if defined(_MSC_VER)

		const UInt64 retVal = __rdtsc();

		return retVal;
#elif defined(AGX_APPLE_IOS)
		return mach_absolute_time();
#elif defined (__GNUC__)  && ! defined(__INTEL_COMPILER) && defined(__ia64__)
		unsigned long long int val;
		__asm__ __volatile__("mov %0=ar.itc" : "=r"(val) :: "memory");
		return(val);
#elif defined(__INTEL_COMPILER) && defined(__ia64__) && !defined(HAVE_TICK_COUNTER)
		return __getReg(_IA64_REG_AR_ITC);
#elif defined(__ECC) && defined(__ia64__) && !defined(HAVE_TICK_COUNTER)
		return __getReg(_IA64_REG_AR_ITC);
#elif (defined(__GNUC__) || defined(__ICC)) && (defined(__i386__) || defined(__x86_64__))  && !defined(HAVE_TICK_COUNTER)

		// "The CPUID instruction serializes the processor pipeline so that all of the preceding 
		// instructions must retire before it begins execution.  ...  This is thought to provide 
		//a more accurate cycle count on the code being measured."  [intel person at forum on intel.com]

		// cpuid writes to eax,ebx,ecx, edx, tell g++ about registers being clobbered
		// in 64bits ebx is 32bits of the larger rbx register so should not require
		// any ifdef's in 64bit
		// would be better to use pushad, cpuid, popad but for some unknow reason gcc
		// doesn't recognize pushad/popad
		// Update: gcc has different name for pushad/popad, l postfix instead of d,
		// but those instructions doesn't work in 64bit

#ifdef __LP64__
		unsigned long long int lowbits, highbits;
		__asm__ __volatile__("rdtsc": "=a" (lowbits), "=d"(highbits) : : "ebx", "ecx");
#else
		unsigned long int lowbits, highbits;
		__asm__ __volatile__("pushal\n\t" "cpuid\n\t" "popal\n\t" "rdtsc": "=a" (lowbits), "=d"(highbits));
		//__asm__ __volatile__("push %%ebx\n" "push %%ecx\n" "cpuid\n" "rdtsc\n" "pop %%ebx\n" "pop %%ecx": "=a" (lowbits), "=d"(highbits));
#endif

		return (((unsigned long long int)highbits << 32) | (unsigned long long int)lowbits);

#else  
		agxAbort1("Timer::getCurrentTick failed!");
		return -1;
#endif

	}

	inline Real64 Timer::getTime(UInt64 startTick)
	{
		return Timer::convertToMs(Timer::getCurrentTick() - startTick);
	}

	inline Real64 Timer::convertToMs(UInt64 ticks)
	{
		return clockSpeed() * (Real64)ticks;
	}



} // namespace vr 
#endif
