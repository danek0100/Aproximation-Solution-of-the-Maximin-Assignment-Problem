#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Point.h"

class Rectangle
{
private:
    double radius;
    Point centre;
    Point leftTop;
    Point leftBottom;
    Point rightTop;
    Point rightBottom;

    void setCornerPoints( double radius ) 
    { 
        leftTop.setX( Common::round_to( centre.getX() - radius ) );
        leftTop.setY( Common::round_to( centre.getY() + radius ) );

        leftBottom.setX( Common::round_to( centre.getX() - radius ) );
        leftBottom.setY( Common::round_to( centre.getY() - radius ) );

        rightTop.setX( Common::round_to( centre.getX() + radius ) );
        rightTop.setY( Common::round_to( centre.getY() + radius ) );

        rightBottom.setX( Common::round_to( centre.getX() + radius ) );
        rightBottom.setY( Common::round_to( centre.getY() - radius ) );

    };

public:
    Rectangle( const Point& centre, double radius )
    {
        setCentre( centre );
        setRadius( radius );
    }

    Rectangle( const Point& leftTop, const Point& rightTop, const Point& leftBottom, const Point& rightBottom )
    {
        this->leftTop = leftTop;
        this->rightTop = rightTop;
        this->leftBottom = leftBottom;
        this->rightBottom = rightBottom;
    }

    const Point& getCentre() const
    {
        return centre;
    }

    const Point& getLeftTop() const
    {
        return leftTop;
    }

    const Point& getLeftBottom() const
    {
        return leftBottom;
    }

    const Point& getRightTop() const
    {
        return rightTop;
    }

    const Point& getRightBottom() const
    {
        return rightBottom;
    }

    void setCentre( const Point& centre )
    {
        this->centre = centre;
    };

    void setRadius( double radius )
    {
        this->radius = radius;
        setCornerPoints( radius );
    }

    void clampRectangle( double xMin, double xMax, double yMin, double yMax)
    {
        leftTop.clamp( xMin, xMax, yMin, yMax );
        leftBottom.clamp( xMin, xMax, yMin, yMax );
        rightTop.clamp( xMin, xMax, yMin, yMax );
        rightBottom.clamp( xMin, xMax, yMin, yMax );
    }

    void print()
    {
        std::cout << "Rectangle with centre: [ " << centre.getX() << ", " << centre.getY() << " ] and radius: "
            << radius << std::endl;
    }

    int inRectange( int x, int y ) 
    {
        int leftX = static_cast<int>( leftTop.getX() );
        int rightX = static_cast<int>( rightTop.getX() );
        int bottomY = static_cast<int>( rightBottom.getY() );
        int topY = static_cast<int>( rightTop.getY() );

        if ( x > leftX && x < rightX && y > bottomY && y < topY ) return 0;

        bool isOnTopLeft = ( x == leftX && y == topY );
        bool isOnTopRight = ( x == rightX && y == topY );
        bool isOnBottomLeft = ( x == leftX && y == bottomY );
        bool isOnBottomRight = ( x == rightX && y == bottomY );

        if ( isOnTopLeft || isOnTopRight || isOnBottomLeft || isOnBottomRight ) return 3;

        bool isOnLeftSide = ( x == leftX && y < topY && y > bottomY );
        bool isOnRightSide = ( x == rightX && y < topY && y > bottomY );
        bool isOnBottom = ( y == bottomY && x > leftX && x < rightX );
        bool isOnTop = ( y == topY && x > leftX && x < rightX );

        if ( isOnLeftSide || isOnRightSide || isOnBottom || isOnTop ) return 2;

        return 1;
    }

    int outRectangeSides( int x, int y )
    {
        int leftX = static_cast<int>( leftTop.getX() );
        int rightX = static_cast<int>( rightTop.getX() );
        int bottomY = static_cast<int>( rightBottom.getY() );
        int topY = static_cast<int>( rightTop.getY() );

        if (x < leftX && y <= topY && y >= bottomY) return 1;
        if (x > rightX && y <= topY && y >= bottomY ) return 2;
        if (y < bottomY && x >= leftX && x <= rightX) return 3;
        if (y > topY && x >= leftX && x <= rightX) return 4;

        return 0;
    }
	
	int inRectangeArea( int x, int y ) 
	{
		int leftX = static_cast<int>( leftTop.getX() );
        int rightX = static_cast<int>( rightTop.getX() );
        int bottomY = static_cast<int>( rightBottom.getY() );
        int topY = static_cast<int>( rightTop.getY() );

        if ( x >= leftX && x <= rightX && y >= bottomY && y <= topY ) return 0;
		retrun 1;
	}
};

#endif // RECTANGLE_H
