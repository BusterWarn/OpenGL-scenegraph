
#include <vr/Timer.h>
#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#pragma comment(lib, "PowrProf.lib") 
#include <windows.h>
#include <PowrProf.h>

#else
#include <time.h>
#endif

using namespace vr;

double Timer::s_clockSpeed = -1;

struct Initializer
{
	Initializer()
	{
		Timer::initClockSpeed();
	}
};

Initializer init;


#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#ifdef _WIN32
#include <PowrProf.h>
#else
#include <unistd.h>
#endif



#ifdef _WIN32
typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION;
#endif

namespace
{
	static unsigned int readNumCpu()
	{
#if (defined _WIN32)
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		return (unsigned int)sysInfo.dwNumberOfProcessors;

#elif (defined __APPLE__)
		uint64_t tmp = 0;
		size_t s = sizeof(tmp);
		sysctlbyname("hw.activecpu", &tmp, &s, NULL, 0);
		return (UInt)(tmp <= 0 ? 1 : tmp);
#else
		// linux, if failure, assume 1 cpu
		long tmp = sysconf(_SC_NPROCESSORS_ONLN);
		return (UInt)(tmp <= 0 ? 1 : tmp);
#endif
	}



	static unsigned int readCpuFrequency()
	{
#ifdef _MSC_VER
		unsigned long value;
		// structure definition was accidentally not included in WinNT.h
		// see: http://msdn2.microsoft.com/en-us/library/aa373184.aspx


		unsigned int numCpu = readNumCpu();
		PROCESSOR_POWER_INFORMATION *ppi = new PROCESSOR_POWER_INFORMATION[numCpu];
		CallNtPowerInformation(ProcessorInformation, NULL, 0, &(ppi[0]), (ULONG)sizeof(PROCESSOR_POWER_INFORMATION)*ULONG(numCpu));
		value = ppi[0].MaxMhz;
		delete[] ppi;
		return (unsigned int)value;

#elif defined(__APPLE__)
		uint64_t tmp = 0;
		size_t s = sizeof(tmp);
		sysctlbyname("hw.cpufrequency", &tmp, &s, NULL, 0);
		return (UInt)((tmp <= 0 ? 0 : tmp) / 1000000.0);
#else
		float cs = 0;
		int done = EOF;
		FILE *fp = fopen("/proc/cpuinfo", "r");
		char buff[1024];
		if (fp) {
			do {
				char* res = fgets(buff, 1024, fp);
				if (res)
					done = sscanf(buff, "cpu MHz  : %f", &cs);
			} while (done == 0 && done != EOF);
			fclose(fp);
		}

		return done != EOF ? (UInt)cs : 0;
#endif
	}



	unsigned int getCpuFrequency()
	{
		static unsigned int s_cpuFrequency = -1;

		if (s_cpuFrequency == -1)
			s_cpuFrequency = readCpuFrequency();

		return s_cpuFrequency;
	}

}


void Timer::initClockSpeed()
{

#ifdef AGX_APPLE_IOS
	mach_timebase_info_data_t timebase;
	mach_timebase_info(&timebase);

	s_clockSpeed = (double)timebase.numer / (double)timebase.denom / double(1000000.0);

#elif defined( __linux__ )

	uint64_t t1, t2;
	uint32_t ms = 10;
	int sleepOk = -1;

	struct timespec c1, c2;

	while (sleepOk != 0)
	{
		struct timespec ts;

		ts.tv_sec = 0;
		ts.tv_nsec = ms * 1000 * 1000;

		clock_gettime(CLOCK_MONOTONIC, &c1);
		t1 = vr::Timer::getCurrentTick();

		sleepOk = nanosleep(&ts, NULL);

		clock_gettime(CLOCK_MONOTONIC, &c2);
		t2 = vr::Timer::getCurrentTick();
	}

	float ms2 = (float)((double)(c2.tv_nsec - c1.tv_nsec) / 1000000.0);
	if (ms2 < 0)
		ms2 += 1000;

	s_clockSpeed = ms2 / ((float)(t2 - t1));

#elif defined(_MSC_VER)
//	LARGE_INTEGER frequency;
//	QueryPerformanceFrequency(&frequency);
//	s_clockSpeed = double(1000.0) / (double)(frequency.QuadPart);
//#else
	unsigned int cpuFrequency = getCpuFrequency();

	if (cpuFrequency != 0)
	{
		s_clockSpeed = double(1.0) / (double)(1000 * cpuFrequency);
	}
	else
	{
		std::cerr << "Could not determine CPU speed, Timer will not function correctly." << std::endl;
		s_clockSpeed = 1.0;
	}
#endif

}


#if defined(_MSC_VER)
UInt64 Timer::getCurrentTickWindows()
{

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	UInt64 retVal = currentTime.QuadPart;
	return retVal;
}

#endif