#ifndef GUROBI_SOLVER_H
#define GUROBI_SOLVER_H

#include "../Solver.h"
#include <gurobi_c++.h>

class GurobiSolver : public Solver
{
public:
	double solve( Task* toSolve, double eps, const std::string& contourAlgorithm="" ) override;
};


#endif // GUROBI_SOLVER_H