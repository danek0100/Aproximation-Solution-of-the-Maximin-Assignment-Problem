#ifndef COMMON_H
#define COMMON_H

#include <cmath>

class Common
{
public:
	static double round_to( double value, double precision = 1 )
	{
		if ( precision == 1 ) return std::round( value );
		return std::round( value / precision ) * precision;
	}
};

#endif // COMMON_H
