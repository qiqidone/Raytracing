#include "RayCommonData.h"

using namespace std;

RayCommonData::RayCommonData()
:m_nThreadNum(32)
{
	vector<RayPixel> tempVector(1920);
	for (int i = 0; i < 1080; ++i)
		scene.push_back(tempVector);
}
