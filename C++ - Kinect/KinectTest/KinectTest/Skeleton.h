#pragma once
#include <Kinect.h>
class Skeleton
{
private:
	Joint joints[JointType_Count];

	HandState leftHandState;
	HandState rightHandState;
	bool tracked;
public:
	Skeleton();
	void updateFromBody(IBody *body);
	bool isTracked() const;
	Joint* getJoints() const;
};