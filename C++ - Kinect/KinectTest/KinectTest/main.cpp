#include "stdafx.h"
#include<string>
#include<iostream>
#include<memory>
#include<SFML\Graphics.hpp>
#include"KinectInterface.h"
#include"KinectFeatures.h"
#include"Skeleton.h"
#include"Bitmask.h"
#include <Windows.h>
#include<serial\serial.h>
#include "main.h"


void writeConfig(serial::Serial &ser, int lSpeed, int rSpeed, bool lDir, bool rDir, bool br)
{
	uint8_t data[] = {
		0xAA,
		200,
		0xBB,
		200,
		0xCC,
		0
	};
	data[1] = lSpeed;
	data[3] = rSpeed;
	data[5] = (rDir&0x1) | (lDir&0x1) << 1 | (br&0x1) << 2;
	ser.write(data,(size_t) 6);
}
void main()
{
	sf::RenderWindow rend(sf::VideoMode(1920, 1080), "Kinect Test");
	KinectInterface kinect;
	try
	{
		kinect.openKinect(KinectFeatures::coordinateMapper | KinectFeatures::bodyStream | KinectFeatures::colorStream | KinectFeatures::depthStream);
	}
	catch (std::exception e)
	{
		std::unique_ptr<wchar_t> wtext(new wchar_t[strlen(e.what()+1)]);
		size_t outsize;
		mbstowcs_s(&outsize,wtext.get(), strlen(e.what()), e.what(), strlen(e.what()) - 1);
		std::cerr << e.what();
		MessageBox(NULL, (LPCWSTR)L"Fehler!", (LPCWSTR)wtext.release() , MB_OK);
	}
	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
	sf::Text text;
	text.setFont(font);
	sf::Sprite spr;
	serial::Serial arduini;
	try
	{
		
		arduini.setStopbits(serial::stopbits_one);
		arduini.setFlowcontrol(serial::flowcontrol_none);
		arduini.setPort("COM7");
		arduini.setBaudrate(115200);
		arduini.open();
	}
	catch (serial::SerialException e)
	{
		std::cerr << e.what();
	}

	while (rend.isOpen())
	{
		sf::Event evt;
		while (rend.pollEvent(evt))
		{
			if (evt.type == sf::Event::Closed)
			{
				rend.close();
			}
		}

		if (arduini.available())
		{
			uint8_t buff;
			arduini.read(&buff, 1);
			std::cout << buff;
		}
		//spr.setTexture(*kinect.getCurrentDepthTexture());
		Skeleton* skel = kinect.getCurrentSkeletons();
		sf::CircleShape shape;
		shape.setRadius(10.f);
		spr.setPosition(0, 0);
		rend.clear();
		//rend.draw(spr);
		text.setCharacterSize(30);
		for (int i = 0; i < 6; i++)
		{
			if (skel[i].isTracked())
			{
				Joint j = skel[i].getJoints()[0];
				int rightSpeed = 0;
				int leftSpeed = 0;
				bool rightDir = true;
				bool leftDir = false;
				bool br;
				float arc = atan2(j.Position.Z, j.Position.X);
				float dist = sqrt(j.Position.Z*j.Position.Z + j.Position.Y*j.Position.Y + j.Position.X * j.Position.X);
				if (dist > 1)
				{
					br = false;
					if (arc - 90 > 20)
					{
						rightDir = true;
						leftDir = false;
						rightSpeed = 10;
						leftSpeed = 10;

					}
					else if (arc - 90 <= -20)
					{
						leftDir = true;
						rightDir = false;
						leftSpeed = 10;
						rightSpeed = 10;
					}
					else
					{
						leftSpeed = 0;
						rightSpeed = 0;
					}
				}
				else
				{
					br = true;
					leftSpeed = 0;
					rightSpeed = 0;
				}
				writeConfig(arduini, leftSpeed, rightSpeed, rightDir, leftDir, br);
				std::cout << "lspeed" << leftSpeed << " rspeed" << rightSpeed << "break" << br << std::endl;
				for (int ii = 0; ii < 25; ii++)
				{
					ColorSpacePoint csp;
					kinect.getCoordinateMapper()->MapCameraPointToColorSpace(skel[i].getJoints()[ii].Position, &csp);
					shape.setPosition(csp.X, csp.Y);
					rend.draw(shape);
				}
				j = skel[i].getJoints()[0];
				std::string str;
				/*str += "X"; 
				str += std::to_string(j.Position.X);
				str += "\nY";
				str += std::to_string(j.Position.Y);
				str += "\nZ";
				str += std::to_string(j.Position.Z);*/
				arc = atan2(j.Position.Z, j.Position.X);
				dist = sqrt(j.Position.Z*j.Position.Z + j.Position.Y*j.Position.Y + j.Position.X * j.Position.X);
				str = std::to_string((arc/3.141)*180) + " " + std::to_string(dist) + "m";
				ColorSpacePoint csp;
				kinect.getCoordinateMapper()->MapCameraPointToColorSpace(j.Position, &csp);
				text.setPosition(csp.X+20, csp.Y-50);
				text.setString(str);
				rend.draw(text);
				//
			}
		}
		rend.display();
	}
}