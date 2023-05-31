#ifndef FIXOBJECT_H
#define FIXOBJECT_H

#include "../Rectangle/Point.h"
#include <iostream>
#include <algorithm>

class FixObject
{
private:
    int index;
    Point place;
    int weight;

public:
    FixObject( int index, Point place, int weight ) : index( index ), place( place ), weight( weight ) {};

    int getIndex()
    { 
        return index;
    }

    Point getPoint()
    {
        return place;
    }

    int getWeight()
    {
        return weight;
    }
    
    // Print information about the FixObject
    void print()
    {
        std::cout << "Fix object " << index << ": x=" << place.getX() << "; y=" << place.getY() << "; weight=" << weight << ";" << std::endl;
    }

    // Calculate the current zone radius based on L, weight, and restriction
    double getCurrentZoneRadius( double L, double restriction )
    {
        return std::max( Common::round_to( L / weight ), restriction );
    }
};

#endif // FIXOBJECT_H