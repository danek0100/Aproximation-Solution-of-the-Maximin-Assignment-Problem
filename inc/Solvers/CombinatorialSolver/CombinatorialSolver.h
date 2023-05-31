#ifndef COMBINATORIAL_SOLVER_H
#define COMBINATORIAL_SOLVER_H

#include "../Solver.h"
#include <unordered_map>

class CombinatorialSolver : public Solver
{
private:
	Task* toSolve;
	double leftBoard;
	double rightBoard;
	double L;
	bool needClamp = true;
	std::unique_ptr<std::vector<Point>> ( *contourAlgorithm )( std::vector<Rectangle>* rectangles, Task* toSolve );
	std::unique_ptr<Rectangle> allowedArea;

public:
	double solve( Task* toSolve, double eps, const std::string& contourAlgorithm ) override;

	bool boardsInit();
	bool solveMultiFacilitiesRecognitionProblem( unsigned facility );
	bool solveRecognitionProblem( FreeObject& firstFreeObject, FreeObject& secondFreeObject );
	std::unique_ptr<std::vector<Rectangle>> generateRectanglesByFreeObject( FreeObject& freeObject );
	static std::unique_ptr<std::vector<Point>> get—ontourWithCoveredLines( std::vector<Rectangle>* rectangles, Task* toSolve );
	double p( Point& p1, Point& p2 );
	std::pair<int, Point> checkCondition( const Point& p, std::vector<Rectangle>* constraints );

};


#endif // COMBINATORIAL_SOLVER_H