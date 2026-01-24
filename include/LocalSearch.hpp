#ifndef LOCAL_SEARCH_HPP
#define LOCAL_SEARCH_HPP

#include "genetic.hpp"
#include "instance.hpp"
#include "random.hpp"

class LocalSearch 
{
    private:
        Instance instance{};
        
    public:
        LocalSearch(Instance instance);
        Individual iteratedGreedy(Individual giant_tour, int destruction_rate);

        double fitnessFunction(std::vector<int> tour);
        void calcCumulativeDistance(std::vector<int>&offspring, std::vector<double>&cumulative_distance);

        double calcDistanceNEH(std::vector<int>&tour, double bestFitness, int index, int idClienteAdd);

        std::vector<int> opt_2(std::vector<int> tour, int i, int j);
        double neh(std::vector<int>&tour, std::vector<int> indices, double best_fitness);
        std::vector<std::tuple<double, double, double>> redePert(std::vector<int>&tour);
};

#endif