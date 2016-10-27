#pragma once

#include <Kinect.h>
#include <SFML\Graphics.hpp>
#include "Skeleton.h"
#include"KinectFeatures.h"
#include "Bitmask.h"
#include <memory>

class KinectInterface
{
private:
	IKinectSensor*			kinectSensor;
	ICoordinateMapper*      coordinateMapper = NULL;
	// Body reader
	IBodyFrameReader*       bodyFrameReader = NULL;
	IColorFrameReader*      colorFrameReader = NULL;
	IDepthFrameReader*		depthFrameReader = NULL;
	RGBQUAD*                depthRGBX = NULL;
	RGBQUAD*				colorRGBX = NULL;
	sf::Texture*			colorImage;
	sf::Texture*			depthImage;
	Skeleton*				skeletons;
	KinectFeatures			usedFeatures;
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	USHORT nDepthMinReliableDistance = 0;
	USHORT nDepthMaxDistance = 0;
	
public:
	
	KinectInterface();
	~KinectInterface();
	void openKinect(KinectFeatures features);
	sf::Texture* getCurrentColorTexture() ;
	sf::Texture* getCurrentDepthTexture() ;
	Skeleton* getCurrentSkeletons();
	ICoordinateMapper* getCoordinateMapper() const;
};