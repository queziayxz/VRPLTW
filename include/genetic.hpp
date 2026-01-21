#ifndef GENETIC_HPP
#define GENETIC_HPP

#include "instance.hpp"
#include <vector>
#include <utility>

struct Individual {
  std::vector<int> chromosome{};           // vetor com os ids dos clientes (giant tour)
  double fitness = 0.0;                    // valor da funcao objetivo para aquele individuo
};

struct Route {
  std::vector<int> customers{};           // vetor com os ids dos clientes visitados na ordem do vetor ?
  std::vector<int> assigned_lockers{};    // vetor com as atribuicoes aos lockers para cada cliente, se o cliente nao usou o locker entao eh -1
  double total_distance = 0.0;            // total de distancia das rotas
  double load = 0.0;                      // demanda atual da rota - nao pode exceder a capacidade do veiculo
};

struct Solution {
  std::vector<Route> routes{};            // vetor com as rotas da solucao
  double total_distance = 0.0;            // total de distancia de todas as rotas
};

typedef std::vector<Individual> Population;

auto random_individual(const Instance& instance) -> Individual;
auto initialize_population(const Instance& instance, unsigned population_size) -> Population;
auto select_parents(const Population& population) -> std::pair<const Individual&, const Individual&>;
auto crossover(const Individual& parent1, const Individual& parent2) -> Individual;
auto mutate(Individual& individual, double mutation_rate) -> void;
auto evaluate_fitness(const std::vector<Route>& routes) -> double;
auto decode_individual(const Instance& instance, const Individual& individual) -> std::vector<Route>;
auto try_locker(double vehicle_capacity, const Point& prev_position, const Client& client, 
  const std::vector<double>& locker_remaining_capacities, const std::vector<Locker>& lockers,
  double current_time, double current_load, double& new_time
) -> int;
auto can_serve_client(double vehicle_capacity, const Point& prev_position, const Client& client, const Depot& depot,
  double current_time, double current_load, double& new_time
) -> bool;
auto genetic_algorithm(const Instance& instance, unsigned population_size, unsigned generations, double mutation_rate) -> Solution;

#endif // GENETIC_HPP