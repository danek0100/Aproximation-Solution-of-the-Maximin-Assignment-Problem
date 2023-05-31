#ifndef FREEOBJECT_H
#define FREEOBJECT_H

#include "../Rectangle/Point.h"
#include <iostream>

class FreeObject
{
private:
	int index;
	Point place;
	bool fixed = false;

public:
	FreeObject( int index ) : index( index ) {};

	int getIndex()
	{
		return index;
	}

	bool isFixed()
	{
		return fixed;
	}

	void setFixed( bool isFixed )
	{
		this->fixed = isFixed;
	}

	void setPoint( Point point )
	{
		this->place = point;
	}

	Point getPlace()
	{
		return place;
	}

	void print()
	{
		std::cout << "Free object " << index << std::endl;
	}
};

#endif // FREEOBJECT_H