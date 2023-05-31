#ifndef ENTITY_H
#define ENTITY_H

#include "Chromosome.h"

class Entity
{
public:
    Chromosome chromosome;
    long suitability;
    bool allowed;

    Entity(Chromosome& chromosome) : chromosome(std::move(chromosome))
    {
        std::pair<long, bool> params = this->chromosome.calculateSuitability();
        this->suitability = params.first;
        this->allowed = params.second;
    }

    Chromosome& getChromosome()
    {
        return chromosome;
    }

    long getSuitability() const
    {
        return suitability;
    }

    bool isAllowed() const
    {
        return allowed;
    }

    bool operator==(const Entity& other) const
    {
        return chromosome == other.chromosome &&
            suitability == other.suitability &&
            allowed == other.allowed;
    }
};


#endif // ENTITY_H