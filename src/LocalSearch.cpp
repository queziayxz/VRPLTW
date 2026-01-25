#include <iostream>
#include <limits>

#include "LocalSearch.hpp"
#include "point.hpp"
#include "random.hpp"

const double INF = std::numeric_limits<double>::infinity();

LocalSearch::LocalSearch(Instance instance)
{
    this->instance = instance;
}

Individual LocalSearch::iteratedGreedy(Individual individual, int destruction_rate)
{
    Individual best = individual;
    double best_fitness = fitnessFunction(individual.chromosome);
    

    Individual offspring = individual;
    std::vector<int> indices((int)destruction_rate);
    int it = 0;
    int maxIter = 1;
    int index = 0;
    double fitness = 0;

    while(it < maxIter) {
        
        offspring = best;
        
        //destroi
        // for(int i = 0; i < destruction_rate; i++) {
        //     index = Random::get_int(0u, (unsigned)offspring.chromosome.size()-1u);
        //     indices[i] = offspring.chromosome[index];
        //     offspring.chromosome.erase(offspring.chromosome.begin()+index);
        // }

        // reconstroi com neh
        // fitness = neh(offspring.chromosome,indices,best_fitness);

        // faz busca local com 2-opt best improvement
        for(int i = 0; i < offspring.chromosome.size(); i++) {
            for(int j = i+1; j < offspring.chromosome.size(); j++) {
                individual.chromosome = opt_2(offspring.chromosome,i,j);
                fitness = fitnessFunction(individual.chromosome);
                if(fitness < best_fitness) {
                    best_fitness = fitness;
                    best = individual;
                    break;
                }
            }
        }
        it++;

    }

    // std::cout << "retornou busca" << std::endl;

    // std::cout << "fitness depois da busca local: " << best_fitness << std::endl;
    // std::cout << "giant tour depois busca local: " << std::endl;
    // for(int gen : best.chromosome) {
    //     std::cout << gen << " ";
    // }
    // std::cout << std::endl;
    return best;

}

double LocalSearch::neh(std::vector<int>&tour, std::vector<int> indices, double total_distance)
{
    double aux_fitness = 0;
    double best_indice = 1;
    double best_fitness = INF;

    std::vector<std::tuple<double, double, double>> rede_pert = redePert(tour);

    for(int i = 0; i < indices.size(); i++) {
        for(int j = 1; j < tour.size()-1; j++) { // so verificar para o meio da tour e nao as extremidades
            aux_fitness = calcDistanceNEH(tour,total_distance,j,indices[i]);
            if(aux_fitness < best_fitness) {
                best_fitness = aux_fitness;
                best_indice = j;
            }
        }
        tour.insert(tour.begin()+best_indice+1,indices[i]);
        total_distance = best_fitness;
        best_fitness = INF;
    }

    return total_distance;
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

std::vector<std::tuple<double, double, double>> LocalSearch::redePert(std::vector<int>& tour) {
    // tuple: <TempoInicio, Espera, Folga>
    std::vector<std::tuple<double, double, double>> rede(tour.size());

    // --- 1. FORWARD PASS (Início e Espera) ---
    double chegada;
    
    // Primeiro Cliente (Depósito -> C1)
    chegada = ::distance(this->instance.depot.position, this->instance.clients.at(tour[0]).position);
    double t_inicio = std::max(chegada, this->instance.clients.at(tour[0]).time_window.start);
    double espera = t_inicio - chegada; // Se chegou em 60 e abre 306, espera 246
    rede[0] = std::make_tuple(t_inicio, espera, 0.0);

    for (int i = 1; i < tour.size(); i++) {
        // Chegada = Inicio do anterior + distância
        chegada = std::get<0>(rede[i-1]) + ::distance(this->instance.clients.at(tour[i-1]).position, this->instance.clients.at(tour[i]).position);
        
        t_inicio = std::max(chegada, this->instance.clients.at(tour[i]).time_window.start);
        espera = t_inicio - chegada; // Espera é sempre Início - Chegada
        
        rede[i] = std::make_tuple(t_inicio, espera, 0.0);
    }

    // --- 2. BACKWARD PASS (Folga / Slack) ---
    // A folga do último cliente é simplesmente o quanto falta para ele fechar
    int n = tour.size() - 1;
    double folga_propria = this->instance.clients.at(tour[n]).time_window.end - std::get<0>(rede[n]);
    std::get<2>(rede[n]) = folga_propria;

    for (int i = n - 1; i >= 0; i--) {
        folga_propria = this->instance.clients.at(tour[i]).time_window.end - std::get<0>(rede[i]);
        
        // A folga acumulada é o mínimo entre a minha folga e o que o próximo aguenta
        // O próximo aguenta: a espera dele (amortecedor) + a folga dele
        double espera_proximo = std::get<1>(rede[i+1]);
        double folga_proximo = std::get<2>(rede[i+1]);
        
        std::get<2>(rede[i]) = std::min(folga_propria, espera_proximo + folga_proximo);
    }

    // --- 3. PRINT ---
    for (int i = 0; i < tour.size(); i++) {
        std::cout << "Cliente: " << tour[i] 
                  << " | T: " << std::get<0>(rede[i]) 
                  << " | Espera: " << std::get<1>(rede[i]) 
                  << " | Folga: " << std::get<2>(rede[i]) << std::endl;
    }
}
double LocalSearch::calcDistanceNEH(std::vector<int>&tour, double total_distance, int index, int idClienteAdd)
{
    if(index == 0 || index == tour.size()-1) return total_distance;

    Client clientA = this->instance.clients.at(tour[index]);
    Client clientB = this->instance.clients.at(tour[index+1]);
    Client clientAdd = this->instance.clients.at(idClienteAdd);

    
    double distance_aresta = ::distance(clientA.position,clientB.position);
    // std::cout << "distance " << clientA.id << " -> " << clientB.id << ": " << distance_aresta << std::endl;
    // std::cout << "best fitess antes: " << total_distance << std::endl;
    total_distance -= distance_aresta;
    // std::cout << "best fitess depois de retirar aresta: " << total_distance << std::endl;
    
    total_distance += ::distance(clientA.position,clientAdd.position);
    // std::cout << "best fitess depois de adicionar aresta A: " << total_distance << std::endl;
    total_distance += ::distance(clientB.position,clientAdd.position);
    // std::cout << "best fitess depois de adicionar aresta B: " << total_distance << std::endl;

    return total_distance;
}

double LocalSearch::fitnessFunction(std::vector<int> tour)
{
    double distance = 0.0;

    for(int i = 0; i < tour.size()-1; i++) {
        distance += ::distance(this->instance.clients.at(tour[i]).position,
                this->instance.clients.at(tour[i+1]).position);
    }

    return distance;
}

// double evaluate_fitness(const std::vector<Route>& routes) {
//   auto total_distance = 0.0;

//   for (int i = 0; i < routes.size(); i++) {
//     total_distance += routes[i].total_distance;
//     for(int j = 0; j < routes[i].customers.size(); j++) {
//       if(routes[i].assigned_lockers[j] > 0) {
//         total_distance += ::distance(instance.clients.at(routes[i].customers[j]).position,instance.lockers.at(routes[i].assigned_lockers[j]).position);
//       }
//     }
//   }
//   return total_distance;
// }