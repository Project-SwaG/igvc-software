#include "EncoderTracking.hpp"


EncoderTracking::EncoderTracking(OSMC_4wd_driver * driver)
{
x=0;
y=0;
angle=M_PI/2;
osmc=driver;
}

EncoderTracking::EncoderTracking()
{
x=0;
y=0;
angle=M_PI/2;
}

void EncoderTracking::reset()
{
	x=0;
	y=0;
	angle=M_PI/2;
}

void EncoderTracking::setTo(double X, double Y, double Angle)
{
	x=X;
	y=Y;
	angle=Angle;
}

double EncoderTracking::getX()
{
	return x;
}

double EncoderTracking::getY()
{
	return y;
}

double EncoderTracking::getAngle()
{
	return angle;
}

double EncoderTracking::getBearing()
{
	return 450-angle;
}

void EncoderTracking::update()
{
	double distanceRightF;
	double distanceLeftF;
	double distanceRightB;
	double distanceLeftB;
	(*osmc).getEncoderDist(distanceLeftF, distanceRightF, distanceLeftB, distanceRightB);
	double distanceRight=(distanceRightF+distanceRightB)/2;
	double distanceLeft=(distanceLeftB+distanceLeftF)/2;
	double distanceCenter=(distanceLeft+distanceRight)/2;
	angle+=(distanceRight-distanceLeft)/(2*WHEEL_BASE);
	x+=distanceCenter*sin(angle);
	y+=distanceCenter*cos(angle);
}


