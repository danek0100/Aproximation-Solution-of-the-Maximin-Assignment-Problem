#include "../../inc/Solvers/CombinatorialSolver/CombinatorialSolver.h"
#include "../../inc/Solvers/CombinatorialSolver/CornerPointFinders/SegmentTree/RectangleUnionContourFinder.h"
#include "../../inc/Solvers/CombinatorialSolver/CornerPointFinders/GeneticAlgorithm/GAContourFinder.h"
#include "../../inc/Solvers/CombinatorialSolver/CornerPointFinders/SegmentTree/SharedTreeContourFinder.h"

#include <algorithm>
#include <string>

typedef std::pair<std::pair<double, Point>, std::pair<double, Point>> Segment;

double CombinatorialSolver::solve( Task* toSolve, double eps, const std::string& contourAlgorithm )
{
	this->toSolve = toSolve;
	if (contourAlgorithm == "coveredLines")
	{
		this->contourAlgorithm = static_cast<std::unique_ptr<std::vector<Point>>(*)( std::vector<Rectangle>* rectangles, Task* toSolve )>( reinterpret_cast<void*>( &(get—ontourWithCoveredLines) ) );
	}
	else if (contourAlgorithm == "segmentTree")
	{
		this->contourAlgorithm = static_cast<std::unique_ptr<std::vector<Point>>(*)( std::vector<Rectangle> * rectangles, Task * toSolve )>( reinterpret_cast<void*>( &(RectangleUnionContourFinder::get—ontourWithSegmentTree) ) );
	}
	else if (contourAlgorithm == "genetic")
	{
		this->contourAlgorithm = static_cast<std::unique_ptr<std::vector<Point>>(*)( std::vector<Rectangle> * rectangles, Task * toSolve )>(reinterpret_cast<void*>( &(GAFinder::getCandidatesWithGA) ) );
	}
	else if (contourAlgorithm == "sharedSegmentTree")
	{
		this->contourAlgorithm = static_cast<std::unique_ptr<std::vector<Point>>(*)(std::vector<Rectangle> * rectangles, Task * toSolve)>(reinterpret_cast<void*>(&(SharedTreeContourFinder::get—ontourWithSegmentTree)));
	}
	else
	{
		throw std::exception( "Bad contour algorithm!" );
	}
	this->needClamp = false;




	Rectangle constArea = { { 0.0, toSolve->getHeigh() }, { toSolve->getWidth() , toSolve->getHeigh() }, { 0.0, 0.0 }, { toSolve->getWidth(), 0.0 } };
	allowedArea = std::make_unique<Rectangle>(constArea);

	if ( !boardsInit() ) return 0.0;

	int steps = 0;
	while ( rightBoard - leftBoard > eps) 
	{
		L = ( leftBoard + rightBoard ) / 2;
		if ( solveMultiFacilitiesRecognitionProblem( toSolve->getFreeObjectNum() ) )
		{
			leftBoard = L;
		}
		else 
		{
			rightBoard = L;
		}
		for ( FreeObject& freeObject : toSolve->getFreeObjects() ) freeObject.setFixed(false);
		steps++;
	}

	return ( leftBoard + rightBoard ) / 2;
}

bool CombinatorialSolver::boardsInit()
{
	try 
	{
		leftBoard = toSolve->getFixObject(0).getWeight() * toSolve->getConstraint( 0, 0 );
		for ( unsigned int fixIndex = 0; fixIndex < toSolve->getFixObjectNum(); ++fixIndex )
		{
			for ( unsigned int freeIndex = 0; freeIndex < toSolve->getFreeObjectNum(); ++freeIndex )
			{
				if ( leftBoard > toSolve->getFixObject( fixIndex ).getWeight() * toSolve->getConstraint( fixIndex, freeIndex ) )
					 leftBoard = toSolve->getFixObject( fixIndex ).getWeight() * toSolve->getConstraint( fixIndex, freeIndex );
			}
		}

		double maxWeight = 0.0;
		for ( unsigned int fixIndex = 0; fixIndex < toSolve->getFixObjectNum(); ++fixIndex )
		{
			if ( toSolve->getFixObject( fixIndex ).getWeight() > maxWeight )
				maxWeight = toSolve->getFixObject( fixIndex ).getWeight();
		}
		rightBoard = std::max( toSolve->getWidth(), toSolve->getHeigh() ) * maxWeight;
		return true;
	}
	catch ( std::exception ex )
	{
		std::cout << ex.what() << std::endl;
	}
	return false;
}

bool CombinatorialSolver::solveMultiFacilitiesRecognitionProblem( unsigned facility )
{
	if ( facility == 2 ) 
	{
		return solveRecognitionProblem( toSolve->getFreeObject(0), toSolve->getFreeObject(1) );
	}
	else 
	{
		unsigned index = toSolve->getFreeObjectNum() - facility + 2;
		FreeObject& toFix = toSolve->getFreeObject( index );
		std::unique_ptr<std::vector<Rectangle>> rectangles = generateRectanglesByFreeObject( toFix );

		std::unique_ptr<std::vector<Point>> contourPoints = contourAlgorithm( rectangles.get(), toSolve );

		toFix.setFixed( true );
		for ( auto& cornerPoint : *contourPoints )
		{
			// œÓ‚ÂˇÂÏ, ˜ÚÓ ÚÓ˜Í‡ Û‰‚ÓÎÂÚ‚ÓˇÂÚ Ó„‡ÌË˜ÂÌË˛ ‡ÁÏÂÌÓÒÚË Á‡‰‡˜Ë
			if ( allowedArea->inRectangeArea( static_cast<int>( cornerPoint.getX() ), static_cast<int>( cornerPoint.getY() ) ) != 0 ) continue;

			toFix.setPoint( cornerPoint );
			if ( solveMultiFacilitiesRecognitionProblem( facility - 1 ) ) 
			{
				return true;
			}
		}
		toFix.setPoint({ -1., -1. });
		toFix.setFixed(false);
		return false;
	}
}

bool CombinatorialSolver::solveRecognitionProblem( FreeObject& firstFreeObject, FreeObject& secondFreeObject )
{
	std::unique_ptr<std::vector<Rectangle>> firstObjectRectangles = generateRectanglesByFreeObject( firstFreeObject );
	std::unique_ptr<std::vector<Rectangle>> secondObjectRectangles = generateRectanglesByFreeObject( secondFreeObject );

	std::unique_ptr<std::vector<Point>> cornerPointsFirst = contourAlgorithm( firstObjectRectangles.get(), toSolve );
	std::unique_ptr<std::vector<Point>> cornerPointsSecond = contourAlgorithm( secondObjectRectangles.get(), toSolve );

	Point uOpt1, uOpt2;
	double pOpt = 0.0;
	for ( auto& cp1 : *cornerPointsFirst ) 
	{
		if (toSolve->isPointOnTaskCorner(cp1)) continue;

		if ( allowedArea->inRectangeArea( static_cast<int>( cp1.getX() ), static_cast<int>( cp1.getY() ) ) != 0 ) continue;

		for ( auto& cp2 : *cornerPointsSecond ) 
		{
			if (allowedArea->inRectangeArea( static_cast<int>( cp2.getX() ), static_cast<int>( cp2.getY()) ) != 0 ) continue;
			if ( pOpt < p( cp1, cp2 ) ) 
			{
				pOpt = p( cp1, cp2 );
				uOpt1 = cp1;
				uOpt2 = cp2;
			}
		}
	}
	if ( pOpt > toSolve->getMutalRestriction( firstFreeObject.getIndex(), secondFreeObject.getIndex() )) 
	{
		return true;
	}
	else 
	{
		return false;
	}
	return true;
}

std::unique_ptr<std::vector<Rectangle>> CombinatorialSolver::generateRectanglesByFreeObject( FreeObject& freeObject )
{
	std::vector<Rectangle> rectangles;
	for ( auto fixObject : toSolve->getFixObjects() ) 
	{
		rectangles.emplace_back( fixObject.getPoint(), fixObject.getCurrentZoneRadius( L, toSolve->getConstraint( fixObject.getIndex(), freeObject.getIndex() ) ) );
		if ( needClamp ) rectangles.back().clampRectangle( 0., toSolve->getWidth(), 0., toSolve->getHeigh() );
	}
	for ( auto localFreeObject : toSolve->getFreeObjects() )
	{
		if ( localFreeObject.isFixed() )
		{
			rectangles.emplace_back( localFreeObject.getPlace(), toSolve->getMutalRestriction( localFreeObject.getIndex(), freeObject.getIndex() ) );
			if ( needClamp ) rectangles.back().clampRectangle( 0., toSolve->getWidth(), 0., toSolve->getHeigh() );
		}
	}
	return std::move( std::make_unique<std::vector<Rectangle>>( rectangles ) );
}


std::unique_ptr<std::vector<Point>> CombinatorialSolver::get—ontourWithCoveredLines( std::vector<Rectangle>* rectangles, Task* toSolve )
{
	std::vector<std::pair<double, Rectangle>> rectanglesAbscissae;
	for ( unsigned i = 0; i < rectangles->size(); ++i )
	{
		rectanglesAbscissae.emplace_back( (*rectangles)[i].getLeftTop().getX(), (*rectangles)[i] );
		rectanglesAbscissae.emplace_back( (*rectangles)[i].getRightTop().getX(), (*rectangles)[i] );
	}

	std::sort(rectanglesAbscissae.begin(), rectanglesAbscissae.end(),
		[](const auto& leftElement, const auto& rightElement)
		{
			if (leftElement.first != rightElement.first) {
				return leftElement.first < rightElement.first;
			}
			return (leftElement.second.getLeftTop().getX() == leftElement.first) && (rightElement.second.getRightTop().getX() == rightElement.first);
		});

	auto cornerPoints = std::make_unique< std::vector<Point>>();
	std::vector<std::pair<Segment, std::vector<Segment>>> coveredPlace;
	std::vector<Point> points;
	std::vector<Point> added;
	std::vector<Point> deleted;

	unsigned long i = 0;
	while (i < rectangles->size() * 2) 
	{
		double X = rectanglesAbscissae[i].first;

		if ( rectanglesAbscissae[i].first == rectanglesAbscissae[i].second.getLeftTop().getX() )
		{
			auto savedIterator = coveredPlace.begin();
			bool concatenate = false;
			bool needUpdate = false;
			for ( auto iterator = coveredPlace.begin(); iterator < coveredPlace.end(); ++iterator )
			{
				if ( rectanglesAbscissae[i].second.getLeftBottom().getY() < iterator->first.first.first && rectanglesAbscissae[i].second.getLeftTop().getY() >= iterator->first.first.first )
				{
					concatenate = true;

					if ( rectanglesAbscissae[i].second.getLeftTop().getY() > iterator->first.second.first )
					{
						deleted.push_back( iterator->first.second.second );
						iterator->first.second.first = rectanglesAbscissae[i].second.getLeftTop().getY();
						iterator->first.second.second = rectanglesAbscissae[i].second.getLeftTop();
						added.push_back(iterator->first.second.second);
					}

					deleted.push_back( iterator->first.first.second );
					iterator->first.first.first = rectanglesAbscissae[i].second.getLeftBottom().getY();
					iterator->first.first.second = rectanglesAbscissae[i].second.getLeftBottom();
					iterator->second.push_back({ { rectanglesAbscissae[i].second.getLeftBottom().getY(), rectanglesAbscissae[i].second.getLeftBottom() }, { rectanglesAbscissae[i].second.getLeftTop().getY(), rectanglesAbscissae[i].second.getLeftTop() } });
					added.push_back(iterator->first.first.second);

					savedIterator = iterator;

					needUpdate = true;
					break;
				}

				else if ( rectanglesAbscissae[i].second.getLeftTop().getY() > iterator->first.second.first && rectanglesAbscissae[i].second.getLeftBottom().getY() <= iterator->first.second.first )
				{
					concatenate = true;

					if ( rectanglesAbscissae[i].second.getLeftBottom().getY() < iterator->first.first.first )
					{
						deleted.push_back( iterator->first.first.second );
						iterator->first.first.first = rectanglesAbscissae[i].second.getLeftBottom().getY();
						iterator->first.first.second = rectanglesAbscissae[i].second.getLeftBottom();
						added.push_back(iterator->first.first.second);
					}

					deleted.push_back( iterator->first.second.second );
					iterator->first.second.first = rectanglesAbscissae[i].second.getLeftTop().getY();
					iterator->first.second.second = rectanglesAbscissae[i].second.getLeftTop();
					iterator->second.push_back({ { rectanglesAbscissae[i].second.getLeftBottom().getY(), rectanglesAbscissae[i].second.getLeftBottom() }, { rectanglesAbscissae[i].second.getLeftTop().getY(), rectanglesAbscissae[i].second.getLeftTop() } });
					added.push_back( iterator->first.second.second );

					savedIterator = iterator;

					needUpdate = true;
					break;
				}
				else if ( ( rectanglesAbscissae[i].second.getLeftTop().getY() <= iterator->first.second.first ) && ( rectanglesAbscissae[i].second.getLeftBottom().getY() >= iterator->first.first.first ) ) 
				{
					iterator->second.push_back( { { rectanglesAbscissae[i].second.getLeftBottom().getY(), rectanglesAbscissae[i].second.getLeftBottom() }, { rectanglesAbscissae[i].second.getLeftTop().getY(), rectanglesAbscissae[i].second.getLeftTop() } } );
					concatenate = true;
					break;
				}
			}

			if ( !concatenate )
			{
				coveredPlace.push_back( { { { rectanglesAbscissae[i].second.getLeftBottom().getY(), rectanglesAbscissae[i].second.getLeftBottom() }, { rectanglesAbscissae[i].second.getLeftTop().getY(), rectanglesAbscissae[i].second.getLeftTop() } }, { { { rectanglesAbscissae[i].second.getLeftBottom().getY(), rectanglesAbscissae[i].second.getLeftBottom() }, { rectanglesAbscissae[i].second.getLeftTop().getY(), rectanglesAbscissae[i].second.getLeftTop() } } } } );
				added.push_back( { X, rectanglesAbscissae[i].second.getLeftBottom().getY() } );
				added.push_back( { X, rectanglesAbscissae[i].second.getLeftTop().getY() } );
			}
			else if (needUpdate)
			{
				std::vector<std::vector<std::pair<Segment, std::vector<Segment>>>::iterator> toDelete;

				for (auto iterator = coveredPlace.begin(); iterator < coveredPlace.end(); ++iterator)
				{
					if (iterator == savedIterator)
						continue;

					if (savedIterator->first.first.first < iterator->first.first.first && savedIterator->first.second.first >= iterator->first.first.first
						|| savedIterator->first.second.first > iterator->first.second.first && savedIterator->first.first.first <= iterator->first.second.first
						|| (savedIterator->first.second.first <= iterator->first.second.first) && (savedIterator->first.first.first >= iterator->first.first.first))
					{
						for (auto innerIterator : iterator->second)
						{
							savedIterator->second.push_back(innerIterator);
						}

						if (savedIterator->first.first.first > iterator->first.first.first)
						{
							deleted.push_back(savedIterator->first.first.second);
							savedIterator->first.first = iterator->first.first;
						}
						else
						{
							deleted.push_back(iterator->first.first.second);
						}

						if (savedIterator->first.second.first < iterator->first.second.first)
						{
							deleted.push_back(savedIterator->first.second.second);
							savedIterator->first.second = iterator->first.second;
						}
						else
						{
							deleted.push_back(iterator->first.second.second);
						}
						coveredPlace.erase(iterator);
						iterator = coveredPlace.begin();
					}
				}				
			}
		}
		else
		{
			bool wasConcatenate = false;
			for ( auto iterator = coveredPlace.begin(); iterator < coveredPlace.end(); iterator++ )
			{
				if ( rectanglesAbscissae[i].second.getRightBottom().getY() >= iterator->first.first.first && rectanglesAbscissae[i].second.getRightTop().getY() <= iterator->first.second.first )
				{
					wasConcatenate = true;

					deleted.push_back(iterator->first.first.second);
					deleted.push_back(iterator->first.second.second);


					for ( auto innerIterator = iterator->second.begin(); innerIterator < iterator->second.end(); innerIterator++ )
					{
						if ( rectanglesAbscissae[i].second.getRightBottom().getY() == innerIterator->first.first && rectanglesAbscissae[i].second.getRightTop().getY() == innerIterator->second.first )
						{
							iterator->second.erase(innerIterator);
							break;
						}
					}
	
					std::vector<std::pair<Segment, std::vector<Segment>>> updatedCoveredPlace;
					for (auto innerIterator = iterator->second.begin(); innerIterator < iterator->second.end(); innerIterator++)
					{

						bool concatenate = false;
						for (auto updateIterator = updatedCoveredPlace.begin(); updateIterator < updatedCoveredPlace.end(); updateIterator++)
						{
							if (innerIterator->first.first < updateIterator->first.first.first && innerIterator->second.first >= updateIterator->first.first.first)
							{
								concatenate = true;

								if (innerIterator->second.first > updateIterator->first.second.first)
								{
									updateIterator->first.second.first = innerIterator->second.first;
									updateIterator->first.second.second = innerIterator->second.second;
								}

								updateIterator->first.first.first = innerIterator->first.first;
								updateIterator->first.first.second = innerIterator->first.second;

								updateIterator->second.push_back({ { innerIterator->first.first, innerIterator->first.second }, { innerIterator->second.first, innerIterator->second.second } });

								updateIterator = updatedCoveredPlace.begin();
							}

							else if (innerIterator->second.first > updateIterator->first.second.first && innerIterator->first.first <= updateIterator->first.second.first)
							{
								concatenate = true;

								if (innerIterator->first.first < updateIterator->first.first.first)
								{
									updateIterator->first.first.first = innerIterator->first.first;
									updateIterator->first.first.second = innerIterator->first.second;
								}

								updateIterator->first.second.first = innerIterator->second.first;
								updateIterator->first.second.second = innerIterator->second.second;

								updateIterator->second.push_back({ { innerIterator->first.first, innerIterator->first.second }, { innerIterator->second.first, innerIterator->second.second } });

								updateIterator = updatedCoveredPlace.begin();
							}
							else if ((innerIterator->second.first <= updateIterator->first.second.first) && (innerIterator->first.first >= updateIterator->first.first.first))
							{
								updateIterator->second.push_back({ { innerIterator->first.first, innerIterator->first.second }, { innerIterator->second.first, innerIterator->second.second } });
								concatenate = true;
								break;
							}
						}

						if (!concatenate)
						{
							updatedCoveredPlace.push_back({ { { innerIterator->first.first, innerIterator->first.second }, { innerIterator->second.first, innerIterator->second.second } }, { { { innerIterator->first.first, innerIterator->first.second }, { innerIterator->second.first, innerIterator->second.second } } } });
						}
					}

					coveredPlace.erase(iterator);
					for (auto newPlace : updatedCoveredPlace)
					{
						coveredPlace.push_back(newPlace);
						added.push_back(newPlace.first.first.second);
						added.push_back(newPlace.first.second.second);
					}
					break;
				}
			}

			if ( !wasConcatenate )
			{
				double lby = rectanglesAbscissae[i].second.getRightBottom().getY();
				double lty = rectanglesAbscissae[i].second.getRightTop().getY();
				for ( auto iterator = coveredPlace.begin(); iterator < coveredPlace.end(); iterator++ )
					if ( lby == iterator->first.first.first && lty == iterator->first.second.first )
					{
						coveredPlace.erase(iterator);
						break;
					}

				deleted.push_back( { X, rectanglesAbscissae[i].second.getRightBottom().getY() } );
				deleted.push_back( { X, rectanglesAbscissae[i].second.getRightTop().getY() } );
			}
		}


		if ( ( i + 1 < rectangles->size() * 2 ) && ( rectanglesAbscissae[i].first != rectanglesAbscissae[i + 1].first ) )
		{
			auto pointsCopy{ points };


			for ( auto point : added ) points.push_back( point );
			for ( auto point : deleted )
			{
				for ( auto iterator = points.begin(); iterator < points.end(); iterator++ )
					if ( point.getY() == iterator->getY() ) 
					{ 
						points.erase( iterator );
						break;
					};
			}

			
			for (auto& newPoint : points)
			{
				bool was = false;
				for ( auto& point : pointsCopy )
				{
					if ( point.getY() == newPoint.getY() )
					{
						was = true;
						break;
					}
				}
				if (!was) cornerPoints->emplace_back( X, newPoint.getY() );
			}


			for (auto& point : pointsCopy)
			{
				bool was = false;
				for (auto& newPoint : points)
				{
					if (point.getY() == newPoint.getY())
					{
						was = true;
						break;
					}
				}
				if (!was) cornerPoints->emplace_back(X, point.getY());
			}

			deleted.clear();
			added.clear();
		}

		if ( i + 1 == rectangles->size() * 2 )
		{
			for ( auto point = deleted.begin(); point < deleted.end(); point++ )
			{
				cornerPoints->emplace_back( X, point->getY() );
			}
			points.clear();
		}
		++i;
	}
	return std::move( cornerPoints );
}



double CombinatorialSolver::p( Point& p1, Point& p2 )
{
	double dx = p1.getX() - p2.getX();
	double dy = p1.getY() - p2.getY();
	return std::sqrt( dx * dx + dy * dy );
}

std::pair<int, Point> CombinatorialSolver::checkCondition( const Point& p, std::vector<Rectangle>* constraints )
{
	int position = allowedArea->inRectange( static_cast<int>(p.getX()), static_cast<int>(p.getY()) );
	if (position != 1) return { 1, {} };
	else
	{
		int canMove = allowedArea->outRectangeSides( static_cast<int>(p.getX()), static_cast<int>(p.getY()));
		if ( canMove == 0 ) return { 0, {} };
		Point moveTo;
		if (canMove == 1) moveTo = { 0.0, p.getY() };
		else if (canMove == 2) moveTo = { toSolve->getWidth(), p.getY() };
		else if (canMove == 3) moveTo = { p.getX(), 0.0 };
		else if (canMove == 4) moveTo = { p.getX(), toSolve->getHeigh() };

		for ( auto& rectangle : *(constraints) )
		{
			if ( rectangle.inRectange( static_cast<int>(moveTo.getX()), static_cast<int>(moveTo.getY()) ) == 0 ) return { 0, {} };
		}
		return { 2, moveTo };
	}
	return { 0, {} };
}