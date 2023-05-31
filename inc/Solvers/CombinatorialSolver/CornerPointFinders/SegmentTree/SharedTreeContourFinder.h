#ifndef SHARED_TREE_CONTOUR_FINDER_H
#define SHARED_TREE_CONTOUR_FINDER_H

#include "RectangleUnionContourFinder.h"
#include "SegmentTree.h"
#include "../../../../Rectangle/Rectangle.h"
#include "../../../../Common/Common.h"
#include "../../../../Task/Task.h"
#include <algorithm>
#include <utility> 
#include <iostream>


static std::unique_ptr<SegmentTree> segmnetTree = nullptr;


class SharedTreeContourFinder
{
private:
	struct ContourEndPoint
	{
		long x;
		long y;
		long otherY;
		ContourEndPoint() {} //: x(0.), y(0.), otherY(0.) {}
		ContourEndPoint(long x, long y, long otherY) : x(x), y(y), otherY(otherY) {}
	};

	struct VerticalEdge
	{
		long x;
		long bottomY;
		long topY;
		VerticalEdge() {} //: x( 0. ), bottomY( 0. ), topY( 0. ) {}
		VerticalEdge(long x, long bottomY, long topY) : x(x), bottomY(bottomY), topY(topY) {}
	};

	struct HorizontalEdge
	{
		long y;
		long leftX;
		long rightX;
		HorizontalEdge() {} //: y( 0. ), leftX( 0. ), rightX( 0. ) {}
		HorizontalEdge(long y, long leftX, long rightX) : y(y), leftX(leftX), rightX(rightX) {}
	};

	struct Interval
	{
		long begin;
		long end;
		Interval() {}//: begin( 0. ), end( 0. ) {}
		Interval(long begin, long end) : begin(begin), end(end) {}
	};

	struct Edge
	{
		long x;
		short position; // 0 - begin, 1 - end
		long beginY;
		long endY;
		Edge() {} //: x(x), position(position), beginY(beginY), endY(endY) {}
		Edge(long x, short position, long beginY, long endY) : x(x), position(position), beginY(beginY), endY(endY) {}
	};

	static void accumulateVerticalEdgesAndEndpoints(std::vector<ContourEndPoint>& A, std::vector<VerticalEdge>& vertical_edges, long abscissa, std::vector<long>& stack)
	{
		unsigned long iOrdinate = 0;
		while (!stack.empty() && iOrdinate < stack.size() - 1)
		{
			if (stack[iOrdinate] < stack[iOrdinate + 1] + 1)
			{
				A.emplace_back(abscissa, stack[iOrdinate], stack[iOrdinate + 1]);
				A.emplace_back(abscissa, stack[iOrdinate + 1], stack[iOrdinate]);
				vertical_edges.emplace_back(abscissa, stack[iOrdinate], stack[iOrdinate + 1]);
			}
			else
			{
				A.emplace_back(abscissa, stack[iOrdinate], stack[iOrdinate + 1]);
				A.emplace_back(abscissa, stack[iOrdinate + 1], stack[iOrdinate]);
				vertical_edges.emplace_back(abscissa, stack[iOrdinate + 1], stack[iOrdinate]);
			}

			iOrdinate += 2;
		}
	}

	static std::unique_ptr < std::pair < std::vector<VerticalEdge>, std::vector<HorizontalEdge> > > contourOfUnionOfRectangles(const std::vector<Rectangle>& rectangles)
	{
		std::vector<Edge> X;

		long yMin, yMax;
		yMin = yMax = static_cast<long>(rectangles[0].getLeftBottom().getY());

		for (auto& rectangle : rectangles)
		{
			long topY = static_cast<long>(std::round(rectangle.getLeftTop().getY()));
			long bottomY = static_cast<long>(std::round(rectangle.getLeftBottom().getY()));

			X.emplace_back(static_cast<long>(std::round(rectangle.getLeftBottom().getX())), 0, bottomY, topY);
			X.emplace_back(static_cast<long>(std::round(rectangle.getRightBottom().getX())), 1, bottomY, topY);
			if (yMin > bottomY) yMin = bottomY;
			if (yMax < topY) yMax = topY;
		}

		std::sort(X.begin(), X.end(),
			[](const auto& leftElement, const auto& rightElement)
			{
				if (leftElement.x != rightElement.x)
				{
					return leftElement.x < rightElement.x;
				}
				return (leftElement.position == 0) && (rightElement.position == 1);
			});


		segmnetTree->extendTree(yMin, yMax);

		std::vector< ContourEndPoint > A;
		std::vector< HorizontalEdge > horizontalEdges;
		std::vector< VerticalEdge > verticalEdges;

		std::vector<long> stack;
		for (size_t i = 0; i < X.size(); ++i)
		{
			if (X[i].position == 0)
			{
				segmnetTree->root->contr(X[i].beginY, X[i].endY, stack);
				accumulateVerticalEdgesAndEndpoints(A, verticalEdges, X[i].x, stack);
				segmnetTree->root->insert(X[i].beginY, X[i].endY);
			}
			else
			{
				segmnetTree->root->remove(X[i].beginY, X[i].endY);
				segmnetTree->root->contr(X[i].beginY, X[i].endY, stack);
				accumulateVerticalEdgesAndEndpoints(A, verticalEdges, X[i].x, stack);
			}
			stack.clear();
		}

		std::sort(A.begin(), A.end(),
			[](const ContourEndPoint& a, const ContourEndPoint& b)
			{
				return a.y < b.y || (a.y == b.y && a.x < b.x);
			}
		);

		for (size_t k = 0; k < A.size() / 2; ++k)
		{
			if (std::abs(A[2 * k].x - A[2 * k + 1].x) > 0)
			{
				horizontalEdges.push_back(HorizontalEdge(A[2 * k].y, A[2 * k].x, A[2 * k + 1].x));
			}
		}

		return std::move(std::make_unique< std::pair < std::vector<VerticalEdge>, std::vector<HorizontalEdge> > >(std::make_pair(verticalEdges, horizontalEdges)));
	}

public:

	static std::unique_ptr<std::vector<Point>> get—ontourWithSegmentTree( std::vector<Rectangle>* rectangles, Task* toSolve )
	{
		if (segmnetTree == nullptr) segmnetTree = std::make_unique<SegmentTree>(0l, static_cast<long>(toSolve->getHeigh()));

		std::unique_ptr<std::pair<std::vector<VerticalEdge>, std::vector<HorizontalEdge>>> edges = contourOfUnionOfRectangles( *rectangles );

		std::unique_ptr<std::vector<Point>> cornerPoints = std::make_unique< std::vector<Point> >();
		for ( auto& horizontalEdge : edges->second )
		{
			cornerPoints->emplace_back( static_cast<double>( horizontalEdge.leftX ), static_cast<double>( horizontalEdge.y) );
			cornerPoints->emplace_back( static_cast<double>( horizontalEdge.rightX ), static_cast<double>( horizontalEdge.y ) );
		}

		std::sort((*cornerPoints).begin(), (*cornerPoints).end(),
			[]( const auto& leftElement, const auto& rightElement )
			{
				return leftElement.getX() < rightElement.getX();
			}
		);

		return std::move( cornerPoints );
	}
};

#endif // RECTANGLE_UNION_CONTOUR_FINDER_H