// KinectTest.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
/*
#include <Kinect.h>
#include <strsafe.h>
#include <stdio.h>
#include <math.h>
#include <SFML\Graphics.hpp>
IKinectSensor*          m_pKinectSensor=NULL;
ICoordinateMapper*      m_pCoordinateMapper=NULL;
RGBQUAD*                m_pColorRGBX;
static const int        cColorWidth = 1920;
static const int        cColorHeight = 1080;

// Body reader
IBodyFrameReader*       m_pBodyFrameReader=NULL;

IBodyFrame* pBodyFrame = NULL;
IBody* ppBodies[6] = { 0 };

// Color reader
IColorFrameReader*      m_pColorFrameReader=NULL;
sf::Vector2f screenJoints[6][JointType_Count];
Joint joints[6][JointType_Count];
static const int        cDepthWidth = 512;
static const int        cDepthHeight = 424;

int selectedSceleton = -1;

sf::Vector2f BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
	// Calculate the body's position on the screen
	ColorSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToColorSpace(bodyPoint, &depthPoint);

	float screenPointX = static_cast<float>(depthPoint.X );
	float screenPointY = static_cast<float>(depthPoint.Y );

	return sf::Vector2f(screenPointX, screenPointY);
}

void ProcessBody(int nBodyCount, IBody** ppBodies)
{
	HRESULT hr;
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);
			if (selectedSceleton == i && !bTracked)
				selectedSceleton = -1;

			if (SUCCEEDED(hr) && bTracked)
			{
				

				hr = pBody->GetJoints(_countof(joints[i]), joints[i]);
				if (SUCCEEDED(hr))
				{
					float arc = atan2(joints[i][0].Position.Z, joints[i][0].Position.X);
					float dist = sqrt(joints[i][0].Position.Z*joints[i][0].Position.Z + joints[i][0].Position.X*joints[i][0].Position.X);
					printf("%f   ", arc / 3.14159265f * 180);
					printf("%f\n", dist);
					boolean tracked;
					ppBodies[i]->get_IsTracked(&tracked);
					if ((joints[i][JointType_HandRight].Position.Y > joints[i][JointType_Head].Position.Y) && selectedSceleton == -1&&tracked)
					{
						selectedSceleton = i;
					}
					
					for (int ii = 0; ii < JointType_Count; ii++)
					{
						screenJoints[i][ii] = BodyToScreen(joints[i][ii].Position, 1920, 1080);
					}

				}
				
				
			}
		}
	}
}


int main()
{
	HRESULT hr;
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	sf::RenderWindow* rend;
	sf::Texture text;
	text.create(1920, 1080);
	sf::Sprite spr;
	sf::Image img;
	img.create(1920, 1080);
	spr.setTexture(text);
	rend = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Kinect Test");
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		SafeRelease(pBodyFrameSource);

		//Color
		IColorFrameSource* pColorFrameSource = NULL;
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}

		SafeRelease(pColorFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		return E_FAIL;
	}

	while (rend->isOpen())
	{
		sf::Event evt;
		while (rend->pollEvent(evt))
		{
			if (evt.type == sf::Event::Closed)
			{
				rend->close()
					;
			}
		}
		if (!m_pBodyFrameReader)
		{
			return 0;
		}

		IBodyFrame* pBodyFrame = NULL;

		HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

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
				ProcessBody(BODY_COUNT, ppBodies);
			}

			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				SafeRelease(ppBodies[i]);
			}
		}

		SafeRelease(pBodyFrame);

		//Color
		if (!m_pColorFrameReader)
		{
			return 0;
		}

		IColorFrame* pColorFrame = NULL;

		hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);

		if (SUCCEEDED(hr))
		{
			INT64 nTime = 0;
			IFrameDescription* pFrameDescription = NULL;
			int nWidth = 0;
			int nHeight = 0;
			ColorImageFormat imageFormat = ColorImageFormat_None;
			UINT nBufferSize = 0;
			RGBQUAD *pBuffer = NULL;

			hr = pColorFrame->get_RelativeTime(&nTime);

			if (SUCCEEDED(hr))
			{
				hr = pColorFrame->get_FrameDescription(&pFrameDescription);
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
				hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
			}

			if (SUCCEEDED(hr))
			{
				if (imageFormat == ColorImageFormat_Bgra)
				{
					hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
				}
				
				

					pBuffer = m_pColorRGBX;
					nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
					hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Rgba);
				
			}

			if (SUCCEEDED(hr))
			{
				int height;
				int width;
				pFrameDescription->get_Height(&height);
				pFrameDescription->get_Width(&width);
				sf::CircleShape c;
				c.setRadius(10.f);
				text.update((sf::Uint8*)pBuffer,(unsigned int) 1920,(unsigned int) 1080,(unsigned int) 0,(unsigned int) 0);
				spr.setTexture(text);
				spr.setPosition(0, 0);
				rend->clear(sf::Color::Black);
				rend->draw(spr);
				
				for (int skel = 0; skel < 6; skel++)
				{
					if (skel == selectedSceleton)
					{
						c.setFillColor(sf::Color::Red);
					}
					else
						c.setFillColor(sf::Color::White);
					for (int i = 0; i < 26; i++)
					{
						
						c.setPosition(screenJoints[skel][i]);
						rend->draw(c);
					}
				}
				
				rend->display();
				height = pBuffer[0].rgbGreen;
				printf("%d", height);

			}

			SafeRelease(pFrameDescription);
		}

		SafeRelease(pColorFrame);
	}
    return 0;
}

*/