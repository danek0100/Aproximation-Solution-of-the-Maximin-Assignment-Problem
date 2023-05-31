#ifndef SOLVER_H
#define SOLVER_H

#include "../Task/Task.h"
#include "../Rectangle/Rectangle.h"

class Solver
{
public:
	virtual double solve( Task* toSolve, double eps, const std::string& contourAlgorithm ) = 0;
	virtual ~Solver() {}
};

#endif // SOLVER_H