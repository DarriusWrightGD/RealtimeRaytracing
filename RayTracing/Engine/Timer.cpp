#include "Timer.h"

Timer:: Timer(void)
{
	//timer.start.QuadPart = QueryPerformanceCounter(&timer.start);;
	//timer.stop.QuadPart = QueryPerformanceCounter(&timer.stop);;
	timer.start.QuadPart = 0;
	timer.stop.QuadPart = 0;
	QueryPerformanceFrequency(&frequency);
}

Timer::~Timer(void)
{
}

float Timer::LargeIntToSecs(LARGE_INTEGER & L)
{
	return ((float)L.QuadPart / (float)frequency.QuadPart);
}

void Timer:: start()
{
	//timer.start = 0l;
	QueryPerformanceCounter(&timer.start);
}
float Timer:: stop()
{
	QueryPerformanceCounter(&timer.stop);
	LARGE_INTEGER time;
	time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
	return LargeIntToSecs(time);
}

float Timer:: interval()
{
	//timer.start = timer.stop;
	//stop();
	LARGE_INTEGER time;
	time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
	//time.QuadPart = timer.start.QuadPart - timer.stop.QuadPart;
	setFramesPerSec(LargeIntToFrames(time));
	return LargeIntToSecs(time);
}

float Timer::LargeIntToFrames(LARGE_INTEGER & L)
{
	return ( (float)frequency.QuadPart / (float)L.QuadPart);
}

void Timer::setFramesPerSec(float frames)
{
	fps = frames;
}

float Timer:: getFramesPerSec()const
{
	return fps;
}

void   Timer::setStart(LARGE_INTEGER start)
{
	timer.start = start;
}
void   Timer::setStop(LARGE_INTEGER stop)
{
	timer.stop = stop;
}
LARGE_INTEGER  Timer::getStart()const
{
	return timer.start;
}
LARGE_INTEGER  Timer::getStop()const
{
	return timer.stop;
}