#ifndef ANGLEUTILS_H
#define ANGLEUTILS_H

#endif // ANGLEUTILS_H

#include <cmath>

class AngleUtils
{
    public:
    inline static double angleToRads(double angle)
    {
        return angle*(M_PI/180.0);
    }

    inline static double radsToAngle(double rad)
    {
        return rad*(180.0/M_PI);
    }
};
