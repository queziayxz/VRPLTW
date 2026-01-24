#include <iostream>
#include <limits>

#include "LocalSearch.hpp"
#include "point.hpp"

const double INF = std::numeric_limits<double>::infinity();

LocalSearch::LocalSearch(Instance instance)
{
    this->instance = instance;
}

Individual LocalSearch::iteratedGreedy(Individual individual, int destruction_rate)
{
    std::vector<double> cumulative_distance = std::vector<double>(individual.chromosome.size()-destruction_rate);
    Individual best = individual;
    double bestFitness = fitnessFunction(individual);

    Individual offspring = individual;
    std::vector<int> indices((int)destruction_rate);
    int i = 0;
    int y = 0;
    int x = 0;
    int maxIter = 30;

    while(i < maxIter) {
        x = Random::get_int(0u, (unsigned)individual.chromosome.size()-1u);
        
        do {
            y = Random::get_int(0u, (unsigned)individual.chromosome.size()-1u);
        } while(y == x);

        individual.chromosome = opt_2(individual.chromosome,x,y);
        double fitness = fitnessFunction(individual);
        if(fitness < bestFitness) {
            bestFitness = fitness;
            best = individual;
        }
        i++;
    }

    return best;

    // std::cout << "giant tour antes da destruicao: " << std::endl;
    // for(int gen : offspring.chromosome) {
    //     std::cout << gen << " ";
    // }
    // std::cout << std::endl;
    // destroi
    // for(int i = 0; i < destruction_rate; i++) {
    //     index = Random::get_int(0u, (unsigned)individual.chromosome.size()-1u);
    //     std::cout << "index removido: " << index << std::endl;
    //     indices[i] = offspring.chromosome[index];
    //     offspring.chromosome.erase(offspring.chromosome.begin()+index);
    // }

    // std::cout << "giant tour depois de destruir: " << std::endl;
    // for(int gen : offspring.chromosome) {
    //     std::cout << gen << " ";
    // }
    // std::cout << std::endl;
    // calcCumulativeDistance(offspring.chromosome,cumulative_distance);
    // double dist = calcDistanceNEH(offspring.chromosome,cumulative_distance,bestFitness2,indices[0]); // ja deixar que essa funcao altere o cumulated distance
    // if(dist < bestFitness) {
    //     bestFitness = dist; // ter uma forma de mapear os locais onde deverá ser inseridos os indices retirados, para ter que inserir somente ao final
    // }

    // opt_2(offspring.chromosome,0,3);
    
    // //reconstroi
    // for(int i = 0; i < destruction_rate; i++) {
    //     index = Random::get_int(0, individual.chromosome.size()-1);
    //     indices[i] = individual.chromosome[index];
    //     individual.chromosome[index] = -1;
    // }

    // return offspring;

}

void LocalSearch::neh(std::vector<int>&tour, int index, int value)
{
    for(int i = 0; i < tour.size(); i++) {

    }
}

std::vector<int> LocalSearch::opt_2(std::vector<int> tour, int i, int j)
{
    std::vector<int> route = tour;
    int aux = 0;
    int x = i < j ? i : j;
    int y = i > j ? i : j;

    if(x >= 0 && y < route.size()) {
        while(x < y) {
            aux = route[(unsigned)y];
            route[(unsigned)y] = route[(unsigned)x];
            route[(unsigned)x] = aux;
            y--;
            x++;
        }
    }

    return route;
}

double LocalSearch::calcDistanceNEH(std::vector<int>&tour, std::vector<double>&cumulative_distance, double&bestFitness, int index, int value)
{
    std::cout << "cumulative_distance.size(): " << cumulative_distance.size() << std::endl;
    double total_distance = cumulative_distance[cumulative_distance.size()-1]; // pega distancia total da rota
    double distance_aresta = 0.0;
    if(index > 0) {
        distance_aresta = cumulative_distance[index] - cumulative_distance[index+1]; // calcula distancia da aresta que vai retirar
    }

    std::cout << "distancia entre " << tour[index] << " e " << tour[index+1] << ": " << distance_aresta << std::endl;
    std::cout << "index: " << index << std::endl;
    std::cout << "index+1: " << index+1 << std::endl;
    std::cout << "value: " << value << std::endl;
    
    std::cout << "total distance antes de retirar aresta: " << total_distance << std::endl;
    total_distance -= abs(distance_aresta); // retira o valor da aresta 
    std::cout << "total distance depois de retirar aresta: " << total_distance << std::endl;
    total_distance += ::distance(this->instance.clients.at(value).position, this->instance.clients.at(tour[index]).position);
    std::cout << "total distance depois de somar primeira distancia: " << total_distance << std::endl;
    total_distance += ::distance(this->instance.clients.at(value).position, this->instance.clients.at(tour[index+1]).position);
    std::cout << "total distance depois final: " << total_distance << std::endl;
    
    if(total_distance < bestFitness) {
        bestFitness = total_distance;

    }

    return total_distance;
}

void LocalSearch::calcCumulativeDistance(std::vector<int>&offspring, std::vector<double>&cumulative_distance)
{
    // cumulative_distance.clear();
    cumulative_distance[0] = 0;

    std::cout << "giant tour function: " << std::endl;
    for(int gen : offspring) {
        std::cout << gen << " ";
    }

    std::cout << std::endl;

    for(int i = 1; i < offspring.size(); i++) {
        std::cout << "getClientById(this->instance.clients(offspring[i]).id: " << this->instance.clients.at(offspring[i]).id << std::endl;
        std::cout << "getClientById(this->instance,offspring[i-1]).id: " << this->instance.clients.at(offspring[i-1]).id << std::endl;
        cumulative_distance[(unsigned)i]  = cumulative_distance[(unsigned)i-1] + ::distance(this->instance.clients.at(offspring[i-1]).position,
                this->instance.clients.at(offspring[i]).position);
        std::cout << "distancia ate " << this->instance.clients.at(offspring[i]).id << ": " << cumulative_distance[(unsigned)i] << std::endl;;
        std::cout << "i: " << i << std::endl;
    }   
}

double LocalSearch::fitnessFunction(Individual&individual)
{
    double distance = 0.0;

    for(int i = 0; i < individual.chromosome.size()-1; i++) {
        distance += ::distance(this->instance.clients.at(individual.chromosome[i]).position,
                this->instance.clients.at(individual.chromosome[i+1]).position);
    }

    return distance;
}