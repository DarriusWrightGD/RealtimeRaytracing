#pragma once

#include <ctime>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>

using std::rand;

class  Random
{
public:
	
	~Random(void){}
	static Random& getInstance()
	{
		static Random instance;
		return instance;
	}


	inline int getRandomInt(int minRange, int maxRange)
	{return  (minRange+ (rand() % (maxRange- minRange)));}
	inline int getRandomInt(int maxRange)
	{return  getRandomInt(0,maxRange);}
		

	inline float getRandomFloat(float minRange, float maxRange)
	{ return minRange + (float)rand()/((float)RAND_MAX/(maxRange - minRange)); }

private:
	Random(void){}
};
