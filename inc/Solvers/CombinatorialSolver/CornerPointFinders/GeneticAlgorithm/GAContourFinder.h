#ifndef GA_CONTOUR_FINDER_H
#define GA_CONTOUR_FINDER_H

#include "../../../Solver.h"

#include "Gene.h"
#include "Chromosome.h"
#include "Entity.h"
#include "Population.h"

#include <vector>
#include <random>
#include <unordered_map>

class GAContourFinder
{
private:
    Task* toSolve;
    std::vector<Rectangle>* restriction;

    void mutation( Chromosome& chromosome );
    Chromosome crossover( std::pair<Entity, Entity>& entityPair );

    std::unique_ptr<Population> generateFirstGeneration( unsigned entitiesNumber );

    std::pair<Entity, Entity> selection( Population* population, bool best = true, bool random = false );
    static Entity binaryTournamentSelection( Population* population, bool best = true );
    static Entity sliceTournamentSelection( Population* population, bool best = true );
    static Entity proportionalSelection( Population* population, bool best = true );

    std::unique_ptr<Population> generationChange( Population* population );

public:
    std::unique_ptr<std::unordered_map<std::string, Point>> getCandidates( std::vector<Rectangle>* );

    GAContourFinder(Task* task) : toSolve(task), restriction(nullptr) {}
};


class GAFinder
{
public:
    static std::unique_ptr< std::vector<Point> > getCandidatesWithGA( std::vector<Rectangle>* rectangles, Task* toSolve )
    {
        GAContourFinder finder(toSolve);
        std::unique_ptr<std::unordered_map<std::string, Point>> candidates = finder.getCandidates(rectangles);
        std::unique_ptr<std::vector<Point>> answer = std::make_unique< std::vector<Point>>();

        for (auto& candidate : *candidates)
        {
            answer->push_back(candidate.second);
        }

        return std::move(answer);
    }
};

#endif // GA_CONTOUR_FINDER_H
