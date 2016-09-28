#include "util.h"

int util::intRand(int min, int max)
{
	if(max <= min) return min;

	int rnd = (rand() % ((max - min)+1)) + min;

	return rnd;
}

float util::floatRand(float min, float max)
{
	if(max <= min) return min;

	float max_num = (max - min);

	return ((float)( (max_num*rand())/RAND_MAX )) + min;
}

