#include "../../inc/Solvers/CombinatorialSolver/CornerPointFinders/GeneticAlgorithm/GAContourFinder.h"

#include <string>
#include <iostream>
#include <functional>
#include <algorithm>


std::random_device rd;
std::mt19937 gen(rd());

static int widthTask = 0;
static int hightTask = 0;

Chromosome::Chromosome( std::vector<Gene> genes, std::vector <Rectangle>* constaints ) : genes( std::move(genes) ), constaints(constaints) { calculateSuitability(); }


std::pair<long, bool> Chromosome::calculateSuitability()
{
    long suitability = 1;
    bool allowed = false;
    int success = 0;

    if ( constaints && genes.size() >= 2 )
    {
        for ( auto& rectangle : *constaints )
        {
            int pos = rectangle.inRectange( genes[0].getPosition(), genes[1].getPosition() );

            if ( pos == 0 || genes[0].getPosition() <= 0 || genes[1].getPosition() <= 0 || ( genes[0].getPosition() >= widthTask ) || ( genes[1].getPosition() >= hightTask ) ) allowed = false;
            else
            {
                suitability += pos;
                success++;
            }
        }
        if ( success == constaints->size() ) allowed = true;
    }

    suitability = std::min(suitability, static_cast<long>(constaints->size() + 2));

    return { suitability, allowed };
}


void Chromosome::shuffleGene()
{
    std::uniform_int_distribution<> xDistribution( 0, widthTask );
    std::uniform_int_distribution<> yDistribution( 0, hightTask );

    genes[0].position = xDistribution( gen );
    genes[1].position = yDistribution( gen );

    return;
}

std::vector<Gene>& Chromosome::getGenes()
{
    return genes;
}

void Chromosome::addGene( const Gene& item )
{
    genes.push_back( item );
    calculateSuitability();
}


Population::Population( std::vector<Entity> population ) : population( std::move( population ) ) {}

void Population::addEntity( Entity& entity )
{
    auto newParams = entity.chromosome.calculateSuitability();
    entity.allowed = newParams.second;
    entity.suitability = newParams.first;
    population.push_back( entity );
}

std::vector<Entity>& Population::getPopulation()
{
    return population;
}

void Population::printPopulation()
{
    for ( int i = 0; i < population.size(); ++i )
    {
        std::cout << '\n';
        std::cout << "Entity = " << i << " | Suitability = " << population[i].getSuitability() << '\n';
        std::cout << "----------------------------------------------" << '\n';
        population[i].getChromosome().print();
    }
}

void Population::printOneEntity( int i )
{
    std::cout << "Entity = " << i << " | Suitability = " << population[i].getSuitability() << '\n';
    std::cout << "----------------------------------------------" << '\n';
    population[i].getChromosome().print();
}

std::unique_ptr< std::vector<Entity> > Population::getMaxSuitabilityEntites( int currentMaxSuitability )
{
    std::vector<Entity> maxEntities;
    int maxSuitability = currentMaxSuitability;
    for ( auto& entity : population )
    {
        std::pair<long, bool> actualResult = entity.chromosome.calculateSuitability();
        if ( actualResult.second && actualResult.first == maxSuitability )
        {
            maxEntities.push_back( entity );
        }
        else if ( actualResult.second && actualResult.first > maxSuitability )
        {
            maxEntities.clear();
            maxEntities.push_back( entity );
            maxSuitability = entity.getSuitability();
        }
    }
    return std::make_unique< std::vector<Entity> >( maxEntities );
}

double Population::getAverageSuitability()
{
    double sumSuitability = 0.0;
    double numberEntity = static_cast<double>( population.size() );
    for ( const auto& entity : population)
    {
        sumSuitability += entity.getSuitability();
    }
    return sumSuitability / numberEntity;
}

Population Population::copy()
{
    std::vector<Entity> populationCopy = population;
    return Population( populationCopy );
}


void GAContourFinder::mutation( Chromosome& chromosome )
{
    std::uniform_int_distribution<> dist( 0, 1000 );
    int prob = dist(gen);
    if ( prob < 10 )
    {
        std::uniform_int_distribution<> xDistribution( -widthTask + 1, widthTask - 1 );
        std::uniform_int_distribution<> yDistribution( -hightTask + 1, hightTask - 1 );

        int newPosition = chromosome.getGenes()[0].position + xDistribution( gen );
        if ( newPosition < 0 ) newPosition = 0;
        if ( newPosition > widthTask ) newPosition = widthTask;
        chromosome.getGenes()[0].position = newPosition;

        newPosition = chromosome.getGenes()[1].position + yDistribution( gen );
        if ( newPosition < 0 ) newPosition = 0;
        if ( newPosition > hightTask ) newPosition = hightTask;
        chromosome.getGenes()[1].position = newPosition;
    }
}

Chromosome GAContourFinder::crossover( std::pair<Entity, Entity>& entityPair )
{
    std::vector<Gene> newGenes;
    Gene& geneFirst = entityPair.first.getChromosome().getGenes()[0];
    Gene& geneSecond = entityPair.second.getChromosome().getGenes()[0];

    long x = ( geneFirst.getPosition() * entityPair.first.getSuitability() + geneSecond.getPosition() * entityPair.second.getSuitability() ) / ( entityPair.first.getSuitability() + entityPair.second.getSuitability() );
    newGenes.emplace_back(x);

    geneFirst = entityPair.first.getChromosome().getGenes()[1];
    geneSecond = entityPair.second.getChromosome().getGenes()[1];

    long y = ( geneFirst.getPosition() * entityPair.first.getSuitability() + geneSecond.getPosition() * entityPair.second.getSuitability() ) / ( entityPair.first.getSuitability() + entityPair.second.getSuitability() );
    newGenes.emplace_back( y );

    Chromosome newChromosome( newGenes, restriction );

    mutation( newChromosome );
    return newChromosome;
}

std::unique_ptr<Population> GAContourFinder::generateFirstGeneration( unsigned entitiesNumber )
{
    std::unique_ptr<Population> population = std::make_unique<Population>();
    for ( unsigned i = 0; i < entitiesNumber; ++i )
    {
        std::vector<Gene> newGenes;
        newGenes.emplace_back( -1 );
        newGenes.emplace_back( -1 );

        Chromosome chromosome( newGenes, restriction );
        chromosome.shuffleGene();
        Entity entity(chromosome);
        population->addEntity( entity );
    }
    return std::move( population );
}

std::pair<Entity, Entity> GAContourFinder::selection( Population* population, bool best, bool random )
{
    Population populationCopy = population->copy();
    int func = rand() % 3;
    std::function<Entity( Population*, bool )> search;
    if ( func == 0 )
    {
        search = binaryTournamentSelection;
    }
    else if ( func == 1 )
    {
        search = proportionalSelection;
    }
    else
    {
        search = sliceTournamentSelection;
    }

    if ( random )
    {
       search = proportionalSelection;
    }

    Entity firstEntity = search( &populationCopy, best );
    populationCopy.getPopulation().erase( std::remove( populationCopy.getPopulation().begin(), populationCopy.getPopulation().end(), firstEntity ), populationCopy.getPopulation().end() );
    Entity secondEntity = search( &populationCopy, best );
    return std::make_pair( firstEntity, secondEntity );
}

Entity GAContourFinder::binaryTournamentSelection( Population* population, bool best )
{
    int entityNumber = static_cast<int>( population->getPopulation().size() );
    int firstEntity = rand() % entityNumber;
    int secondEntity = rand() % entityNumber;

    while ( firstEntity == secondEntity )
    {
        secondEntity = rand() % entityNumber;
    }
    std::vector<Entity> pair = { population->getPopulation()[firstEntity], population->getPopulation()[secondEntity] };
    std::sort( pair.begin(), pair.end(), [](const Entity& a, const Entity& b) { return a.getSuitability() < b.getSuitability(); } );

    if (best) return pair.back();
    return pair.front();
}

Entity GAContourFinder::sliceTournamentSelection( Population* population, bool best )
{
    std::vector<Entity>& populationVec = population->getPopulation();
    
    if ( populationVec.size() == 1 )
    {
        return populationVec[0];
    }

    int startOffset = 0;
    int endOffset = 1;
    if ( populationVec.size() >= 3 )
    {
        startOffset = rand() % static_cast<int>( populationVec.size() / 2 );
        endOffset = rand() % ( populationVec.size() - startOffset - 1 ) + startOffset + 1;
    }

    std::vector<Entity> subset( populationVec.begin() + startOffset, populationVec.begin() + endOffset );
    std::sort(subset.begin(), subset.end(), []( const Entity& a, const Entity& b ) { return a.getSuitability() < b.getSuitability(); });

    if (best) return subset.back();
    return subset.front();
}

Entity GAContourFinder::proportionalSelection( Population* population, bool best )
{
    std::vector<Entity> populationVec = population->getPopulation();
   
    if ( populationVec.size() == 1 )
    {
        return populationVec[0];
    }

    std::vector<double> wheelOfFortune;
    wheelOfFortune.push_back( populationVec[0].getSuitability() );

    for ( unsigned long i = 1; i < static_cast<int>(populationVec.size()); i++ )
    {
        wheelOfFortune.push_back( populationVec[i].getSuitability() + wheelOfFortune[i - 1] );
    }

    int randomSuitability = rand() % static_cast<int>( wheelOfFortune.back() );
    int i = 0;
    while ( randomSuitability > wheelOfFortune[i] )
    {
        i++;
    }
    return populationVec[i];
}

std::unique_ptr<Population> GAContourFinder::generationChange( Population* population )
{
    Population newPopulation;
    Population populationCopy = population->copy();

    int entityNumber = population->getPopulation().size();
    int oldEntityNumber = static_cast<int>( entityNumber * 0.1 );
    int i = 0;

    // Best entity
    while ( i < static_cast<int>( ( entityNumber - oldEntityNumber ) * 0.7 ) )
    {
        std::pair<Entity, Entity> pair = selection( &populationCopy, true );
        Chromosome newChromosome = crossover( pair );
        Entity newEntity( newChromosome );
        newPopulation.addEntity( newEntity );
        int entityForRemove = rand() % 2;
        populationCopy.getPopulation().erase( populationCopy.getPopulation().begin() + entityForRemove );
        i++;
    }

    // Worst entities
    while ( i < static_cast<int>( ( entityNumber - oldEntityNumber ) * 0.8 ) )
    {
        std::pair<Entity, Entity> pair = selection( &populationCopy, false );
        Chromosome newChromosome = crossover( pair );
        Entity newEntity( newChromosome );
        newPopulation.addEntity( newEntity );
        int entityForRemove = rand() % 2;
        populationCopy.getPopulation().erase( populationCopy.getPopulation().begin() + entityForRemove );
        i++;
    }

    // Random entities
    while ( i < static_cast<int>( ( entityNumber - oldEntityNumber ) * 0.9 ) )
    {
        std::pair<Entity, Entity> pair = selection( &populationCopy, false, true );
        Chromosome newChromosome = crossover( pair );
        Entity newEntity( newChromosome );
        newPopulation.addEntity( newEntity );
        int entityForRemove = rand() % 2;
        populationCopy.getPopulation().erase( populationCopy.getPopulation().begin() + entityForRemove );
        i++;
    }

    // Best old entities
    while ( i < entityNumber )
    {
        Entity oldEntity = binaryTournamentSelection( population );
        newPopulation.addEntity( oldEntity );
        populationCopy.getPopulation().erase( std::remove(populationCopy.getPopulation().begin(), populationCopy.getPopulation().end(), oldEntity ), populationCopy.getPopulation().end() );
        i++;
    }

    return std::move( std::make_unique<Population>( newPopulation ) );
}

std::unique_ptr<std::unordered_map<std::string, Point>> GAContourFinder::getCandidates( std::vector<Rectangle>* restriction )
{
    widthTask = static_cast<int>( toSolve->getWidth() );
    hightTask = static_cast<int>( toSolve->getHeigh() );
    unsigned entityNum = 100;
    unsigned Tm = 1;
    
    this->restriction = restriction;

    std::unique_ptr<Population> population = generateFirstGeneration( entityNum );

    for ( unsigned generationNumber = 0; generationNumber < Tm; ++generationNumber )
    {
        population = generationChange( population.get() );
    }

    std::unordered_map<std::string, Point> answer;
    std::unique_ptr<std::vector<Entity>> candidats = population->getMaxSuitabilityEntites( restriction->size() + 1 );
    for ( auto& entity : *candidats )
    {
        int x = entity.getChromosome().getGenes()[0].getPosition();
        int y = entity.getChromosome().getGenes()[1].getPosition();
        answer[std::to_string(x) + "_" + std::to_string(y)] = { static_cast<double>(x), static_cast<double>(y) };
    }

    return std::move(std::make_unique<std::unordered_map<std::string, Point>>( answer ));
}