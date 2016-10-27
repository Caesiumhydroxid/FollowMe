#include"stdafx.h"
#include "KinectInterface.h"
#include "KinectFeatures.h"


KinectInterface::KinectInterface()
{
	colorImage = new sf::Texture();
	colorImage->create(1920, 1080);
	depthImage = new sf::Texture();
	depthImage->create(cDepthWidth, cDepthHeight);
	skeletons = new Skeleton[6];
	colorRGBX = new RGBQUAD[1920 * 1080];
	depthRGBX = new RGBQUAD[cDepthWidth * cDepthHeight];

}
KinectInterface::~KinectInterface()
{
	kinectSensor->Close();
}
void KinectInterface::openKinect(KinectFeatures features)
{
	this->usedFeatures = features;
	HRESULT hr;
	hr = GetDefaultKinectSensor(&kinectSensor);
	if (this->kinectSensor)
	{
		hr = kinectSensor->Open();
		BOOLEAN isOpen;
		kinectSensor->get_IsOpen(&isOpen);
		if (!isOpen)
		{
			throw std::runtime_error("Cannot open Kinect");
		}
		if (static_cast<bool>(features & KinectFeatures::coordinateMapper))
		{
			hr = kinectSensor->get_CoordinateMapper(&coordinateMapper);
			if (FAILED(hr))
			{
				throw std::runtime_error("Cannot open CoordinateMapper");
			}
		}
		if (static_cast<bool>(features & KinectFeatures::bodyStream))
		{
			IBodyFrameSource* bodyFrameSource = NULL;
			hr = kinectSensor->get_BodyFrameSource(&bodyFrameSource);
			if (FAILED(hr))
				throw std::runtime_error("Cannot open BodyFrameSource");
			hr = bodyFrameSource->OpenReader(&bodyFrameReader);
			SafeRelease(bodyFrameSource);
			if (FAILED(hr))
				throw std::runtime_error("Cannot open BodyReader");
		}
		if (static_cast<bool>(features&KinectFeatures::colorStream))
		{
			IColorFrameSource* colorFrameSource = NULL;
			hr = kinectSensor->get_ColorFrameSource(&colorFrameSource);
			if (FAILED(hr))
				throw std::runtime_error("Cannot open ColorFrameSource");
			hr = colorFrameSource->OpenReader(&colorFrameReader);
			SafeRelease(colorFrameSource);
			if (FAILED(hr))
				throw std::runtime_error("Cannot open ColorFrameReader");
		}
		if (static_cast<bool>(features&KinectFeatures::depthStream))
		{
			IDepthFrameSource* depthFrameSource;
			hr = kinectSensor->get_DepthFrameSource(&depthFrameSource);
			if (FAILED(hr))
				throw std::runtime_error("Cannot open DepthFrameSource");
			hr = depthFrameSource->OpenReader(&depthFrameReader);
			SafeRelease(depthFrameSource);
			if (FAILED(hr))
				throw std::runtime_error("Cannot open DepthFrameReader");
		}
	}
	else
	{
		throw std::runtime_error("Cannot find default Kinect");
	}
}

sf::Texture* KinectInterface::getCurrentColorTexture()
{
	if (!colorFrameReader)
	{
		throw "Failed to Open Frame Reader";
	}

	IColorFrame* colorFrame = NULL;
	HRESULT hr;
	hr = colorFrameReader->AcquireLatestFrame(&colorFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* frameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		UINT nBufferSize = 0;
		RGBQUAD *buffer = NULL;
		hr = colorFrame->get_RelativeTime(&nTime);
		if (SUCCEEDED(hr))
		{
			hr = colorFrame->get_FrameDescription(&frameDescription);
		}
		if (SUCCEEDED(hr))
		{
			hr = frameDescription->get_Width(&nWidth);
		}
		if (SUCCEEDED(hr))
		{
			hr = frameDescription->get_Height(&nHeight);
		}
		if (SUCCEEDED(hr))
		{
			hr = colorFrame->get_RawColorImageFormat(&imageFormat);
		}
		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = colorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&buffer));
			}
			buffer = colorRGBX;
			nBufferSize = nWidth * nHeight * sizeof(RGBQUAD);
			hr = colorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(buffer), ColorImageFormat_Rgba);
		}
		colorImage->update((sf::Uint8*)buffer, (unsigned int)1920, (unsigned int)1080, (unsigned int)0, (unsigned int)0);
		SafeRelease(colorFrame);
		SafeRelease(frameDescription);
	}
	return colorImage;
}

Skeleton* KinectInterface::getCurrentSkeletons()
{
	IBodyFrame* pBodyFrame = NULL;
	HRESULT hr;
	hr = bodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		hr = pBodyFrame->get_RelativeTime(&nTime);
		IBody* ppBodies[BODY_COUNT] = { 0 };

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < BODY_COUNT; i++)
			{
				skeletons[i].updateFromBody(ppBodies[i]);
			}
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}
		SafeRelease(pBodyFrame);
		return skeletons;
	}
	//else
		//throw "Failed to Open Frame";
	return skeletons;
}

ICoordinateMapper * KinectInterface::getCoordinateMapper() const
{
	return coordinateMapper;
}

sf::Texture* KinectInterface::getCurrentDepthTexture() 
{
	if (!depthFrameReader)
	{
		return NULL;
	}

	IDepthFrame* pDepthFrame = NULL;

	HRESULT hr = depthFrameReader->AcquireLatestFrame(&pDepthFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		UINT nBufferSize = 0;
		UINT16 *pBuffer = NULL;

		hr = pDepthFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
		}

		if (SUCCEEDED(hr))
		{
			// In order to see the full range of depth (including the less reliable far field depth)
			// we are setting nDepthMaxDistance to the extreme potential depth threshold
			nDepthMaxDistance = USHRT_MAX;

			// Note:  If you wish to filter by reliable depth distance, uncomment the following line.
			//// hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxDistance);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		}

		if (SUCCEEDED(hr))
		{
			// Make sure we've received valid data
			if (depthRGBX && pBuffer && (nWidth == cDepthWidth) && (nHeight == cDepthHeight))
			{
				RGBQUAD* pRGBX = depthRGBX;

				// end pixel is start + width*height - 1
				const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

				while (pBuffer < pBufferEnd)
				{
					USHORT depth = *pBuffer;

					// To convert to a byte, we're discarding the most-significant
					// rather than least-significant bits.
					// We're preserving detail, although the intensity will "wrap."
					// Values outside the reliable depth range are mapped to 0 (black).

					// Note: Using conditionals in this loop could degrade performance.
					// Consider using a lookup table instead when writing production code.
					BYTE intensity = static_cast<BYTE>(256-depth);
					pRGBX->rgbRed = intensity;
					pRGBX->rgbGreen = intensity;
					pRGBX->rgbBlue = intensity;
					++pRGBX;
					++pBuffer;
				}
				pRGBX = depthRGBX;
				depthImage->update((sf::Uint8*)pRGBX, cDepthWidth, cDepthHeight, 0, 0);
			}
		}

		SafeRelease(pFrameDescription);
	}
	SafeRelease(pDepthFrame);
	return depthImage;
}
