#pragma once
#include "Bitmask.h"
enum class KinectFeatures
{
	bodyStream = 1 << 0,
	colorStream = 1 << 1,
	coordinateMapper = 1 << 2,
	depthStream = 1 << 3
};

template<>
struct enable_bitmask_operators<KinectFeatures> {
	static const bool enable = true;
};