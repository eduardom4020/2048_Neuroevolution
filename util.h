#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef unsigned int uint;
typedef unsigned char byte;

typedef vector<float> floatv;

struct SVector3D
{
    SVector3D() : x(0), y(0), z(0) {}
    SVector3D(float xi, float yi, float zi) : x(xi), y(yi), z(zi) {}
    float x, y, z;
};

namespace util
{
	int intRand(int min, int max);
	float floatRand(float min, float max);
}

#endif
