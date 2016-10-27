#include "stdafx.h"
#include "Skeleton.h"
#include <Kinect.h>

Skeleton::Skeleton()
{
	tracked = false;
}

void Skeleton::updateFromBody(IBody *body)
{
	body->get_HandLeftState(&this->leftHandState);
	body->get_HandRightState(&this->rightHandState);
	BOOLEAN isTracked;
	body->get_IsTracked(&isTracked);
	this->tracked = isTracked;
	HRESULT hr;
	hr = body->GetJoints(_countof(joints),joints);
}

bool Skeleton::isTracked() const
{
	return tracked;
}

Joint * Skeleton::getJoints() const
{
	return (Joint*)joints;
}
