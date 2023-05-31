#include "../inc/Task/TaskHolder.h"
#include "../inc/Solvers/Solver.h"
#include "../inc/Solvers/Gurobi/GurobiSolver.h"
#include "../inc/Solvers/CombinatorialSolver/CornerPointFinders/GeneticAlgorithm/GAContourFinder.h"
#include "../inc/Solvers/CombinatorialSolver/CombinatorialSolver.h"

#include <iostream>
#include <chrono>

int main(int argc, char** argv)
{
	std::unique_ptr<Solver> solver = nullptr;
	std::unique_ptr<Task> toSolve = nullptr;

	double eps = 1e-3;
	std::string contourRectangleBase = "segmentTree";
	int N = 1;
	
	if ( argc >= 3 )
	{
		if ( std::string(argv[1]) == "combinatorial" )
		{
			solver = std::make_unique<CombinatorialSolver>();
		}
		else if ( std::string(argv[1]) == "gurobi" )
		{
			solver = std::make_unique<GurobiSolver>();
			contourRectangleBase = "gurobi";
		}
		else
		{
			solver = std::make_unique<CombinatorialSolver>();
		}

		if ( std::string(argv[2]) == "generate" )
		{
			toSolve = std::make_unique<Task>( TaskHolder::generateTask() );
			TaskHolder::writeTaskToFile( toSolve.get(), "lastSolvedTask.dat" );
		}
		else if ( std::string(argv[2]) == "parse" && argc >= 4 )
		{
			toSolve = std::make_unique<Task>( TaskHolder::parseTask(argv[3]) );
		}
		else
		{
			toSolve = std::make_unique<Task>( TaskHolder::generateTask() );
			TaskHolder::writeTaskToFile( toSolve.get(), "lastSolvedTask.dat" );
		}

		if (std::string(argv[2]) == "generate" && argc >= 4)
		{
			if (std::string(argv[3]) == "coveredLines")
			{
				contourRectangleBase = "coveredLines";
			}
			else if (std::string(argv[3]) == "genetic")
			{
				contourRectangleBase = "genetic";
			}
			else if (std::string(argv[3]) == "segmentTree")
			{
				contourRectangleBase = "segmentTree";
			}
			else if (std::string(argv[3]) == "sharedSegmentTree")
			{
				contourRectangleBase = "sharedSegmentTree";
			}
		}

		if (std::string(argv[2]) == "generate" && argc >= 5)
		{
			N = atoi(argv[4]);
		}

		if (std::string(argv[2]) == "parse" && argc >= 5)
		{
			if (std::string(argv[4]) == "coveredLines")
			{
				contourRectangleBase = "coveredLines";
			}
			else if (std::string(argv[4]) == "genetic")
			{
				contourRectangleBase = "genetic";
			}
			else if (std::string(argv[4]) == "segmentTree")
			{
				contourRectangleBase = "segmentTree";
			}
			else if (std::string(argv[4]) == "sharedSegmentTree")
			{
				contourRectangleBase = "sharedSegmentTree";
			}
		}

		if (std::string(argv[2]) == "parse" && argc >= 6)
		{
			N = atoi(argv[5]);
		}

	}
	else
	{
		std::cout << "To start use next structure: ( solver [\"Gurobi\", \"Combinatorial\"], task [\"generate\", \"parse\"] ), [ path_to_file, cornerPointAlgorithm [\"segmentTree\", \"sharedSegmentTree\", \"coveredLines\", \"geneticAlgorithm\"] ]" << std::endl << std::endl;
		return 0;
	}

	std::cout << std::endl << "Will be used " << contourRectangleBase << " algorithm base." << std::endl << std::endl;


	long long sum_time = 0;
	int i = 0;
	double L_sum = 0.0;
	while ( i < N )
	{
		if ( N > 1 && std::string(argv[2]) == "generate" )
		{
			toSolve = std::make_unique<Task>(TaskHolder::generateTask());
			TaskHolder::writeTaskToFile(toSolve.get(), "task_" + std::to_string(i) + ".dat");
		}

		if ( N > 1 && std::string(argv[2]) == "parse" )
		{
			toSolve = std::make_unique<Task>(TaskHolder::parseTask("task_" + std::to_string(i) + ".dat"));
		}

		std::cout << "Solving task_" << i + 1 << " started." << std::endl;

		auto startTime = std::chrono::high_resolution_clock::now();
		double resultMetrick = solver->solve(toSolve.get(), eps, contourRectangleBase);

		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		std::cout << "Execution time: " << duration << " milliseconds" << std::endl;

		std::cout << "Algorith find best L: " << resultMetrick << std::endl << std::endl << std::endl;
		sum_time += duration;
		L_sum += resultMetrick;
		++i;
	}

	std::cout << "Average time: " << sum_time / (double)N << std::endl;
	std::cout << "L_sum: " << L_sum << std::endl;

	return 0;
}