#ifndef POINT_H
#define POINT_H

#include "../Common/Common.h"

class Point
{
private:
	double x;
	double y;

public:
	Point() { x = 0.; y = 0.; };
	Point( double x, double y ) : x( Common::round_to( x ) ), y( Common::round_to( y ) ) {};

	void setX( double x )
	{
		this->x = Common::round_to( x );
	}

	void setY( double y )
	{
		this->y = Common::round_to( y );
	}

	double getX() const
	{
		return x;
	}

	double getY() const
	{
		return y;
	}

	void clamp( double xMin, double xMax, double yMin, double yMax )
	{
		if ( x < xMin ) x = xMin;
		if ( x > xMax ) x = xMax;
		if ( y < yMin ) y = yMin;
		if ( y > yMax ) y = yMax;
	}

	bool equals(const Point& other) const
	{ 
		return ( x == other.x && y == other.y );
	}
};

#endif // POINT_H
