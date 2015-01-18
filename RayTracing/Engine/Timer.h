#pragma once

#include <Windows.h>
#include <ExportHeader.h>
typedef struct{
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
} stopWatch;

class Timer
{
public:
     ENGINE_SHARED Timer(void);
	 ENGINE_SHARED ~Timer(void);
	 ENGINE_SHARED void start();
	 ENGINE_SHARED float stop();
	 ENGINE_SHARED float interval();
	 ENGINE_SHARED float getFramesPerSec()const;
	 ENGINE_SHARED void setStart(LARGE_INTEGER start);
	 ENGINE_SHARED void setStop(LARGE_INTEGER stop);
	 ENGINE_SHARED LARGE_INTEGER getStart()const;
	 ENGINE_SHARED LARGE_INTEGER getStop()const;

private:
	stopWatch timer;
	LARGE_INTEGER frequency;
	float fps;
	float LargeIntToSecs(LARGE_INTEGER & l);
	float LargeIntToFrames(LARGE_INTEGER & l);
	void setFramesPerSec(float frames);
};
