#ifndef CHROMOSOME_H
#define CHROMOSOME_H


#include "Gene.h"
#include "../../../Solver.h"

#include <vector>
#include <iostream>

class Chromosome
{
private:
    std::vector<Gene> genes; // В рамках текущей задачи размерность всегда == 2.
    std::vector<Rectangle>* constaints;

public:
    Chromosome(std::vector<Gene> genes, std::vector<Rectangle>* constaints);

    std::pair<long, bool> calculateSuitability();
    void shuffleGene();

    std::vector<Gene>& getGenes();
    void addGene(const Gene& item);

    bool operator==(const Chromosome& other) const
    {
        return genes == other.genes;
    }

    void print() const
    {
        std::cout << "Chromosome: [ ";
        for (auto& gene : genes)
        {
            std::cout << gene.getPosition() << ", ";
        }
        std::cout << "]" << std::endl;
    }
};


#endif // CHROMOSOME_H