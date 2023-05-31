#include "../../inc/Task/TaskHolder.h"
#include "time.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

static int i = 0;

int generateInRange(int min, int max)
{
	return ( std::abs(rand()) % ( ( max - min ) + 1 ) ) + min;
}

Task TaskHolder::generateTask() 
{
	srand(time(0) + i);
	i += 1;

	double X = static_cast<double>( generateInRange( minX, maxX ) );
	double Y = static_cast<double>( generateInRange( minY, maxY ) );
	int fixObjectNum = generateInRange( minFixObjectNum, maxFixObjectNum );
	int freeObjectNum = generateInRange (minFreeObjectNum, maxFreeObjectNum );

	Task toSolve( X, Y, freeObjectNum, fixObjectNum );

	for ( int i = 0; i < fixObjectNum; ++i )
	{
		toSolve.addFixObject( { i, { static_cast<double>( generateInRange( 0, X ) ), static_cast<double>( generateInRange( 0, Y ) ) }, generateInRange( minWeight, maxWeight ) } );
	}

	for ( int i = 0; i < freeObjectNum; ++i )
	{
		toSolve.addFreeObject({ i });
	}

	for ( int i = 0; i < fixObjectNum; ++i )
	{
		std::vector<double> fixObjectConstaraints;

		for ( int j = 0; j < freeObjectNum; ++j )
		{
			fixObjectConstaraints.emplace_back( Common::round_to( generateInRange( 1, std::min( X, Y ) / constraintDecrisingCoefficent ) + 1. ) );
		}

		toSolve.addNextConstraintLine( fixObjectConstaraints );
	}

	double commonRestriction = Common::round_to( generateInRange( 1, std::min( X, Y ) / constraintDecrisingCoefficent ) + 1. );
	for ( int i = 0; i < freeObjectNum; ++i )
	{
		for ( int j = i + 1; j < freeObjectNum; ++j )
		{
			toSolve.addMutalRestriction(i, j, commonRestriction );
		}
	}

	return toSolve; 
};


Task TaskHolder::parseTask(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open the file: " << filename << std::endl;
		return {0, 0, 0, 0};
    }

    int fixObjectNum = 0;
    int freeObjectNum = 0;
    std::vector<int> weights;
	std::vector<Point> fixCords;
	std::vector<std::vector<double>> restrictCords;
	std::vector<std::vector<double>> mutualRestrictionMatrix;
	double X = 0.;
	double Y = 0.;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::string keyword = line.substr(0, line.find('='));
        if (keyword == "n") {
            fixObjectNum = std::stoi(line.substr(line.find('=') + 1));
        }
        else if (keyword == "p") {
            freeObjectNum = std::stoi(line.substr(line.find('=') + 1));
        }
        else if (keyword == "Weights") {
            // Parse weights
            line = line.substr(line.find('[') + 1);
            line.pop_back();
            size_t pos = 0;
            while ((pos = line.find(',')) != std::string::npos) {
                weights.push_back(std::stoi(line.substr(0, pos)));
                line.erase(0, pos + 1);
            }
            weights.push_back(std::stoi(line));
        }
        else if (keyword == "FixedObjectCoords") {
            // Parse fixed object coordinates
            for (int i = 0; i < fixObjectNum; ++i) {
                std::getline(file, line);
                line = line.substr(line.find('[') + 1);
                line.pop_back();
                double x = std::stod(line.substr(0, line.find(',')));
                double y = std::stod(line.substr(line.find(',') + 1));
				fixCords.emplace_back(x, y);
            }
        }
        else if (keyword == "MinDistances") {
            for (int i = 0; i < fixObjectNum; ++i) {
                std::getline(file, line);
                line = line.substr(line.find('[') + 1);
                line.pop_back();
                std::vector<double> row;
                size_t pos = 0;
                while ((pos = line.find(',')) != std::string::npos) {
                    row.push_back(std::stod(line.substr(0, pos)));
                    line.erase(0, pos + 1);
                }
                row.push_back(std::stod(line));
				restrictCords.push_back(row);
            }
        }
        else if (keyword == "MinDistObjects") {
            for (int i = 0; i < freeObjectNum; ++i) {
                std::getline(file, line);
                line = line.substr(line.find('[') + 1);
                line.pop_back();
                std::vector<double> row;
                size_t pos = 0;
                while ((pos = line.find(',')) != std::string::npos) {
                    row.push_back(std::stod(line.substr(0, pos)));
                    line.erase(0, pos + 1);
                }
                row.push_back(std::stod(line));
                mutualRestrictionMatrix.push_back(row);
            }
        }
        else if (keyword == "SourceAreaDimensions") {
            line = line.substr(line.find('[') + 1);
            line.pop_back();
            double x = std::stod(line.substr(0, line.find(',')));
            double y = std::stod(line.substr(line.find(',') + 1));
            X = x;
            Y = y;
        }
    }
    file.close();

	Task task(X-1, Y-1, freeObjectNum, fixObjectNum);
	for (unsigned i = 0; i < mutualRestrictionMatrix.size(); ++i)
	{
		for (unsigned j = 0; j < mutualRestrictionMatrix.size(); ++j)
		{
			task.addMutalRestriction(i, j, mutualRestrictionMatrix[i][j]);
		}
	}

	for (unsigned i = 0; i < restrictCords.size(); ++i)
	{
		task.addNextConstraintLine(restrictCords[i]);
	}

	for (int i = 0; i < freeObjectNum; ++i) task.addFreeObject({i});
	for (int i = 0; i < fixObjectNum; ++i) task.addFixObject({ i, fixCords[i], weights[i] });

	return task;
};

void TaskHolder::writeTaskToFile(Task* task, const std::string& filename)
{
	std::ofstream file(filename);

	file << "n=" << task->getFixObjectNum() << ";\n";
	file << "p=" << task->getFreeObjectNum() << ";\n";

	file << "Weights=[";
	for (auto& fixObj : task->getFixObjects())
	{
		file << fixObj.getWeight() << ",";
	}
	file.seekp(-1, std::ios_base::end);
	file << "];\n";

	file << "FixedObjectCoords=[\n";
	for (auto& fixObj : task->getFixObjects())
	{
		file << "[" << fixObj.getPoint().getX() << ", " << fixObj.getPoint().getY() << "],\n";
	}
	file.seekp(-3, std::ios_base::end);
	file << "];\n";


	file << "MinDistances=[\n";
	for (auto& fixObj : task->getFixObjects())
	{
		file << "[";
		for (auto& freeObject : task->getFreeObjects())
		{
			file << task->getConstraint(fixObj.getIndex(), freeObject.getIndex()) << ",";
		}
		file.seekp(-1, std::ios_base::end);
		file << "],\n";
	}
	file.seekp(-3, std::ios_base::end);
	file << "];\n";

	file << "MinDistObjects=[\n";
	for (auto& freeObject_1 : task->getFreeObjects())
	{
		file << "[";
		for (auto& freeObject_2 : task->getFreeObjects())
		{
			file << task->getMutalRestriction(freeObject_1.getIndex(), freeObject_2.getIndex()) << ",";
		}
		file.seekp(-1, std::ios_base::end);
		file << "],\n";
	}
	file.seekp(-3, std::ios_base::end);
	file << "];\n";

	file << "SourceAreaDimensions=[" << task->getWidth()+1 << ", " << task->getHeigh()+1 << "];\n";

	file.close();
}