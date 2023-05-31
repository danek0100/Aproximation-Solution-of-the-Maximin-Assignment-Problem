#include "../../../inc/Solvers/Gurobi/GurobiSolver.h"
#include <iostream>
#include <chrono>

double GurobiSolver::solve(Task* toSolve, double eps, const std::string& contourAlgorithm)
{
    double answer = 0.;

    unsigned p = toSolve->getFreeObjectNum();
    unsigned n = toSolve->getFixObjectNum();

    long A = static_cast<long>((toSolve->getWidth()));
    long B = static_cast<long>((toSolve->getHeigh()));
    long W = 2 * ( A + B );
    long H = W;
    long M = static_cast<long>( toSolve->getMutalRestriction( 0, 1 ) );

    std::vector< std::vector<long> > FixedObjectCoords(n);
    std::vector< std::vector<long> > MinDistances(n);
    std::vector<long> Weights(n);

    int i = 0;
    for ( FixObject& fixedObject : toSolve->getFixObjects() )
    {
        FixedObjectCoords[i].push_back(static_cast<long>(fixedObject.getPoint().getX()));
        FixedObjectCoords[i].push_back(static_cast<long>(fixedObject.getPoint().getY()));
        Weights[i] = fixedObject.getWeight();

        MinDistances[i] = std::vector<long>( p );
        for ( unsigned j = 0; j < p; ++j )
        {
            MinDistances[i][j] = static_cast<long>( toSolve->getConstraint( i, j ) );
        }
        ++i;
    }

    try
    {
        // Создаём окружение
        GRBEnv env = GRBEnv( true );
        //env.set("LogFile", "mip1.log");
        env.set(GRB_IntParam_OutputFlag, 0);

        env.start();

        // Создаём модель
        GRBModel model = GRBModel( env );

        // Создаём пременные задачи
        GRBVar L = model.addVar( 0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "L" );

        std::vector<GRBVar> x(p), y(p);
        std::vector< std::vector<GRBVar> > s(p);
        std::vector< std::vector<GRBVar> > u(p);

        for ( unsigned i = 0; i < p; ++i ) 
        {
            x[i] = model.addVar( -GRB_INFINITY, GRB_INFINITY, 0.0, GRB_INTEGER, "x_" + std::to_string(i) );
            y[i] = model.addVar( -GRB_INFINITY, GRB_INFINITY, 0.0, GRB_INTEGER, "y_" + std::to_string(i) );


            s[i] = std::vector<GRBVar>(p);
            u[i] = std::vector<GRBVar>(p);
            for ( unsigned j = 0; j < p; ++j )
            {
                s[i][j] = model.addVar( 0, 1, 0, GRB_BINARY, "s_" + std::to_string(i) + "_" + std::to_string(j) );
                u[i][j] = model.addVar( 0, 1, 0, GRB_BINARY, "u_" + std::to_string(i) + "_" + std::to_string(j) );
            }
        }


        std::vector< std::vector<GRBVar> > z(n);
        std::vector< std::vector<GRBVar> > t(n);

        for ( unsigned i = 0; i < n; ++i )
        {
            z[i] = std::vector<GRBVar>(p);
            t[i] = std::vector<GRBVar>(p);
            for ( unsigned j = 0; j < p; ++j )
            {
                z[i][j] = model.addVar( 0, 1, 0, GRB_BINARY, "z_" + std::to_string(i) + "_" + std::to_string(j) );
                t[i][j] = model.addVar( 0, 1, 0, GRB_BINARY, "t_" + std::to_string(i) + "_" + std::to_string(j) );
            }
        }
        model.update();


        // Задаём ограчения
        model.setObjective( L + 0, GRB_MAXIMIZE );

        for ( unsigned i = 0; i < n; ++ i)
        {
            for ( unsigned j = 0; j < p; ++j )
            {
                model.addConstr( x[j] - FixedObjectCoords[i][0] + y[j] - FixedObjectCoords[i][1] + z[i][j] * H + t[i][j] * W >= MinDistances[i][j] );
                model.addConstr( x[j] - FixedObjectCoords[i][0] + y[j] - FixedObjectCoords[i][1] + z[i][j] * H + t[i][j] * W >= L / Weights[i] );

                model.addConstr( -x[j] + FixedObjectCoords[i][0] + y[j] - FixedObjectCoords[i][1] + (1 - z[i][j]) * H + t[i][j] * W >= MinDistances[i][j] );
                model.addConstr( -x[j] + FixedObjectCoords[i][0] + y[j] - FixedObjectCoords[i][1] + (1 - z[i][j]) * H + t[i][j] * W >= L / Weights[i] );

                model.addConstr( x[j] - FixedObjectCoords[i][0] - y[j] + FixedObjectCoords[i][1] + z[i][j] * H + (1 - t[i][j]) * W >= MinDistances[i][j] );
                model.addConstr( x[j] - FixedObjectCoords[i][0] - y[j] + FixedObjectCoords[i][1] + z[i][j] * H + (1 - t[i][j]) * W >= L / Weights[i] );

                model.addConstr( -x[j] + FixedObjectCoords[i][0] - y[j] + FixedObjectCoords[i][1] + (1 - z[i][j]) * H + (1 - t[i][j]) * W >= MinDistances[i][j] );
                model.addConstr( -x[j] + FixedObjectCoords[i][0] - y[j] + FixedObjectCoords[i][1] + (1 - z[i][j]) * H + (1 - t[i][j]) * W >= L / Weights[i] );
            }
        }

        for ( unsigned i = 0; i < p; ++i )
        {
            for ( unsigned j = 0; j < p; ++j )
            {
                if ( i == j ) continue;
                model.addConstr( x[j] - x[i] + y[j] - y[i] + s[i][j] * H + u[i][j] * W >= M );
                model.addConstr( -x[j] + x[i] + y[j] - y[i] + (1 - s[i][j]) * H + u[i][j] * W >= M );
                model.addConstr( x[j] - x[i] - y[j] + y[i] + s[i][j] * H + (1 - u[i][j]) * W >= M );
                model.addConstr( -x[j] + x[i] - y[j] + y[i] + (1 - s[i][j]) * H + (1 - u[i][j]) * W >= M );
            }
        }


        for ( unsigned j = 0; j < p; ++j )
        {
            model.addConstr( y[j] - x[j] >= 0 );
            model.addConstr( x[j] + y[j] >= 0 );
            model.addConstr( y[j] + x[j] <= A );
            model.addConstr( y[j] - x[j] <= B );
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        // Оптимизируем
        model.optimize();

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        std::cout << "Gurobi optimeze time: " << duration << " milliseconds" << std::endl;


        //for ( unsigned i = 0; i < p; ++i )
        //{
        //    std::cout << "Free object " << i << ":" << std::endl;
        //    std::cout << x[i].get(GRB_StringAttr_VarName) << " " << x[i].get(GRB_DoubleAttr_X) << std::endl;
        //    std::cout << y[i].get(GRB_StringAttr_VarName) << " " << y[i].get(GRB_DoubleAttr_X) << std::endl;
        //    std::cout << std::endl;
        //}

        answer = L.get(GRB_DoubleAttr_X);
    }
    catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    catch (...) {
        std::cout << "Exception during optimization" << std::endl;
    }
    return answer;
}