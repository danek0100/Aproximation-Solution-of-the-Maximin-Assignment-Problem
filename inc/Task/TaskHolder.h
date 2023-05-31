#ifndef TASKHOLDER_H
#define TASKHOLDER_H

#include "Task.h"

class TaskHolder
{
private:
	static constexpr int maxX = 1599;
	static constexpr int maxY = 999;
	static constexpr int minX = 1599;
	static constexpr int minY = 999;

	static constexpr int maxFixObjectNum = 30;
	static constexpr int maxFreeObjectNum = 7;
	static constexpr int minFixObjectNum = 30;
	static constexpr int minFreeObjectNum = 7;

	static constexpr int maxWeight = 10;
	static constexpr int minWeight = 1;

	static constexpr int constraintDecrisingCoefficent = 40;

public:
	TaskHolder() {};
	static Task generateTask();
	static Task parseTask( const std::string& filename );
	static void writeTaskToFile( Task* task, const std::string& filename );
};
#endif // TASKHOLDER_H
