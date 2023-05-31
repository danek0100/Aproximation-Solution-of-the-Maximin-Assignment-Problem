#ifndef POPULATION_H
#define POPULATION_H

#include "Entity.h"

class Population
{
public:
    std::vector<Entity> population;

    Population(std::vector<Entity> population = {});

    void addEntity(Entity& entity);
    std::vector<Entity>& getPopulation();

    void printPopulation();
    void printOneEntity(int i);

    std::unique_ptr<std::vector<Entity>> getMaxSuitabilityEntites(int currentMaxSuitability);
    double getAverageSuitability();

    Population copy();
};


#endif // POPULATION_H

