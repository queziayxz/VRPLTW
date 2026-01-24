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

        double fitnessFunction(Individual&individual);
        void calcCumulativeDistance(std::vector<int>&offspring, std::vector<double>&cumulative_distance);

        double calcDistanceNEH(std::vector<int>&tour, std::vector<double>&cumulative_distance, double&bestFitness, int index, int value);

        std::vector<int> opt_2(std::vector<int> tour, int i, int j);
        void neh(std::vector<int>&tour, int index, int value);
};

#endif