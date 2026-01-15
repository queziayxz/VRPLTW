#ifndef GENETIC_HPP
#define GENETIC_HPP

#include "instance.hpp"
#include <vector>
#include <utility>

struct Individual {
  std::vector<int> chromosome;
  double fitness;
};

struct Route {
  std::vector<int> customers;
  std::vector<int> assigned_lockers;
  double total_distance;
  double load;
};

struct Solution {
  std::vector<Route> routes;
  double total_distance;
};

typedef std::vector<Individual> Population;

auto random_individual(const Instance& instance) -> Individual;
auto initialize_population(const Instance& instance, unsigned population_size) -> Population;
auto select_parents(const Population& population) -> std::pair<const Individual&, const Individual&>;
auto crossover(const Individual& parent1, const Individual& parent2) -> Individual;
auto mutate(Individual& individual, double mutation_rate) -> void;
auto evaluate_fitness(const std::vector<Route>& routes) -> double;
auto decode_individual(const Instance& instance, const Individual& individual) -> std::vector<Route>;
auto try_locker(double vehicle_capacity, const Point& prev_position, const Client& client, const Depot& depot, 
  const std::vector<double>& locker_remaining_capacities, const std::vector<Locker>& lockers,
  double current_time, double current_load, double& new_time
) -> int;
auto can_serve_client(double vehicle_capacity, const Point& prev_position, const Client& client, const Depot& depot,
  double current_time, double current_load, double& new_time
) -> bool;
auto genetic_algorithm(const Instance& instance, unsigned population_size, unsigned generations, double mutation_rate) -> Solution;

#endif // GENETIC_HPP