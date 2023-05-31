#ifndef TASK_H
#define TASK_H

#include "../Common/Common.h"
#include "../Object/FixObject.h"
#include "../Object/FreeObject.h"

#include <vector>
#include <iostream>

class Task
{
private:
	double X;
	double Y;

	unsigned int freeObjectNum;
	unsigned int fixObjectNum;

	std::vector< FixObject > fixObjects;
	std::vector< FreeObject > freeObjects;

	std::vector< std::vector< double > > restrictMatrix;
	std::vector< std::vector< double > > mutualRestrictionMatrix;

public:
	Task( double X, double Y, unsigned freeObjectNum, unsigned fixObjectNum )
		: X( X ), Y( Y ), freeObjectNum( freeObjectNum ), fixObjectNum( fixObjectNum ) 
	{
		mutualRestrictionInit();
	};

	~Task() {};

	double getWidth()
	{
		return X;
	}

	double getHeigh()
	{
		return Y;
	}

	void addFixObject( const FixObject& fixObject ) 
	{ 
		fixObjects.push_back( fixObject );
	}

	void addFreeObject( const FreeObject& freeObject ) 
	{ 
		freeObjects.push_back( freeObject );
	}

	void addNextConstraintLine( const std::vector<double>& restrictLine ) 
	{ 
		restrictMatrix.push_back( restrictLine );
	}

	void mutualRestrictionInit() 
	{ 
		for ( unsigned int i = 0; i < freeObjectNum; ++i )
		{
			std::vector< double > restriction;
			for ( unsigned int j = 0; j < freeObjectNum; ++j )
				restriction.emplace_back( 0 );

			mutualRestrictionMatrix.push_back( restriction );
		}
	}
	
	void addMutalRestriction( int i, int j, double restriction ) 
	{
		mutualRestrictionMatrix[i][j] = Common::round_to( restriction );
		mutualRestrictionMatrix[j][i] = Common::round_to( restriction );
	}

	void printMutalRestrictionMatrix()
	{ 
		for ( unsigned int i = 0; i < freeObjectNum; ++i )
		{
			std::cout << "[ ";
			for ( unsigned int j = 0; j < freeObjectNum; ++j ) 
				std::cout << mutualRestrictionMatrix[i][j] << " ";
			std::cout << "]" << std::endl;
		}
	}

	FixObject& getFixObject( unsigned int index )
	{
		return fixObjects[index];
	}

	FreeObject& getFreeObject( unsigned int index )
	{
		return freeObjects[index];
	}

	unsigned int getFreeObjectNum()
	{
		return freeObjectNum;
	}

	unsigned int getFixObjectNum()
	{
		return fixObjectNum;
	}

	double getConstraint( const unsigned fixObjectIndex, const unsigned freeObjectIndex )
	{
		if ( freeObjectIndex >= 0 && freeObjectIndex < freeObjectNum && fixObjectIndex >= 0 && fixObjectIndex < fixObjectNum )
		{
			return restrictMatrix[fixObjectIndex][freeObjectIndex];
		}
		else return -1;
	}

	double getMutalRestriction( const unsigned freeObjectIndexFirst, const unsigned freeObjectIndexSecond )
	{
		if ( freeObjectIndexFirst >= 0 && freeObjectIndexFirst < freeObjectNum
			&& freeObjectIndexSecond >= 0 && freeObjectIndexSecond < freeObjectNum )
		{
			return mutualRestrictionMatrix[ freeObjectIndexFirst ][ freeObjectIndexSecond ];
		} 
		else return -1;
	}

	std::vector<FixObject>& getFixObjects()
	{
		return fixObjects;
	}

	std::vector<FreeObject>& getFreeObjects()
	{
		return freeObjects;
	}

	bool isPointOnTaskCorner( const Point& point )
	{
		double x = Common::round_to( point.getX() );
		double y = Common::round_to( point.getY() );

		bool isOnTopLeft = ( x == 0.00 && y == Y );
		bool isOnTopRight = ( x == X && y == Y );
		bool isOnBottomLeft = ( x == 0.00 && y == 0.00 );
		bool isOnBottomRight = ( x == X && y == 0.00 );

		return ( isOnTopLeft || isOnTopRight || isOnBottomLeft || isOnBottomRight );
	}
};

#endif // TASK_H
