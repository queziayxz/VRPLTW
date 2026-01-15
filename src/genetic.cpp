#include "genetic.hpp"
#include "random.hpp"
#include <numeric>
#include <algorithm>

#include <iostream>

auto random_individual(const Instance& instance) -> Individual {
  auto individual = Individual{};
  individual.chromosome = std::vector<int>(instance.clients.size());
  std::iota(individual.chromosome.begin(), individual.chromosome.end(), 0);
  std::shuffle(individual.chromosome.begin(), individual.chromosome.end(), Random::mt);
  return individual;
}

auto initialize_population(const Instance& instance, unsigned population_size) -> Population {
  auto population = Population{};

  for (auto i = 0u; i < population_size; ++i) {
    population.push_back(random_individual(instance));
    population.back().fitness = evaluate_fitness(decode_individual(instance, population.back()));
  }

  return population;
}

auto select_parents(const Population& population) -> std::pair<const Individual&, const Individual&> {
  // Tournament selection
  auto tournament_size = 5u;
  const auto* best1 = &population[Random::get_int(0u, (unsigned)population.size() - 1u)];
  for (auto i = 1u; i < tournament_size; ++i) {
    const auto* competitor = &population[Random::get_int(0u, (unsigned)population.size() - 1u)];
    if (competitor->fitness < best1->fitness) {
      best1 = competitor;
    }
  }

  const auto* best2 = &population[Random::get_int(0u, (unsigned)population.size() - 1u)];
  for (auto i = 1u; i < tournament_size; ++i) {
    const auto* competitor = &population[Random::get_int(0u, (unsigned)population.size() - 1u)];
    if (competitor->fitness < best2->fitness) {
      best2 = competitor;
    }
  }

  return {*best1, *best2};
}

auto crossover(const Individual& parent1, const Individual& parent2) -> Individual {
  auto result = Individual{};
  auto chromosome_size = parent1.chromosome.size();
  result.chromosome = std::vector<int>(chromosome_size);

  // Frequency table to track the customers which have been already inserted
	auto freq = std::vector<bool>(chromosome_size, false);

	auto start = Random::get_int(0u, (unsigned)chromosome_size - 1u);
	auto end = Random::get_int(0u, (unsigned)chromosome_size - 1u);

	// Avoid that start and end coincide by accident
	while (end == start) end = Random::get_int(0u, (unsigned)chromosome_size - 1u);

	// Copy from start to end
	auto j = start;
	while (j % chromosome_size != (end + 1) % chromosome_size) {
		result.chromosome[j % chromosome_size] = parent1.chromosome[j % chromosome_size];
		freq[(unsigned)result.chromosome[j % chromosome_size]] = true;
		j++;
	}

	// Fill the remaining elements in the order given by the second parent
	for (auto i = 1u; i <= chromosome_size; ++i) {
		auto temp = parent2.chromosome[(end + i) % chromosome_size];
		if (freq[(unsigned)temp] == false) {
			result.chromosome[j % chromosome_size] = temp;
			j++;
		}
	}

  return result;
}

auto mutate(Individual& individual, double mutation_rate) -> void {
  for (auto i = 0u; i < individual.chromosome.size(); ++i) {
    if (Random::get_real(0.0, 1.0) < mutation_rate) {
      auto j = Random::get_int(0u, (unsigned)individual.chromosome.size() - 1u);
      std::swap(individual.chromosome[i], individual.chromosome[j]);
    }
  }
}

auto evaluate_fitness(const std::vector<Route>& routes) -> double {
  auto total_distance = 0.0;
  for (const auto& route : routes) {
    total_distance += route.total_distance;
  }
  return total_distance;
}

auto can_serve_client(double vehicle_capacity, const Point& prev_position, const Client& client, const Depot& depot,
  double current_time, double current_load, double& new_time
) -> bool {
  if (current_load + client.demand > vehicle_capacity) {
    return false;
  }

  auto arrival = current_time + distance(prev_position, client.position);
  auto start = std::max(arrival, client.time_window.start);

  if (start > client.time_window.end) {
    return false;
  }

  if (start + distance(client.position, depot.position) > depot.time_window.end) {
    return false;
  }

  new_time = start;
  return true;
}

auto try_locker(double vehicle_capacity, const Point& prev_position, const Client& client, const Depot& depot, 
  const std::vector<double>& locker_remaining_capacities, const std::vector<Locker>& lockers,
  double current_time, double current_load, double& new_time
) -> int {
  if (current_load + client.demand > vehicle_capacity) {
    return -1;
  }

  auto best_locker = -1;
  auto best_cost = std::numeric_limits<double>::max();

  for (auto i = 0u; i < lockers.size(); ++i) {
    if (locker_remaining_capacities[i] < client.demand) {
      continue;
    }

    auto cost = distance(prev_position, lockers[i].position);

    if (cost >= best_cost) {
      continue;
    }

    auto arrival = current_time + cost;

    if (arrival + distance(lockers[i].position, depot.position) > depot.time_window.end) {
      continue;
    }

    best_locker = (int)i;
    best_cost = cost;
    new_time = arrival;
  }

  return best_locker;
}

auto decode_individual(const Instance& instance, const Individual& individual) -> std::vector<Route> {
  // Split the chromosome into routes based on vehicle capacity, time windows, and locker assignments
  auto routes = std::vector<Route>{};

  auto locker_remaining_capacities = std::vector<double>(instance.lockers.size());
  for (auto i = 0u; i < instance.lockers.size(); ++i) {
    locker_remaining_capacities[i] = instance.lockers[i].capacity;
  }

  auto current_route = Route{};
  auto current_time = instance.depot.time_window.start;
  auto previous_position = instance.depot.position;

  for (auto i = 0u; i < individual.chromosome.size(); ++i) {
    auto gene = individual.chromosome[i];
    const auto& client = instance.clients[(unsigned)gene];

    if (can_serve_client(instance.vehicle_capacity, previous_position, client, instance.depot, current_time, current_route.load, current_time)) {
      current_route.customers.push_back(gene);
      current_route.assigned_lockers.push_back(-1);
      current_route.load += client.demand;
      current_route.total_distance += distance(previous_position, client.position);
      previous_position = client.position;
      continue;
    } 

    auto locker = try_locker(instance.vehicle_capacity, previous_position, client, instance.depot,
      locker_remaining_capacities, instance.lockers, current_time, current_route.load, current_time);

    if (locker != -1) {
      current_route.customers.push_back(gene);
      current_route.assigned_lockers.push_back(locker);
      current_route.load += client.demand;
      current_route.total_distance += distance(previous_position, instance.lockers[(unsigned)locker].position);
      previous_position = instance.lockers[(unsigned)locker].position;
      locker_remaining_capacities[(unsigned)locker] -= client.demand;
      continue;
    }

    routes.push_back(current_route);
    current_route = Route{};
    current_time = instance.depot.time_window.start;
    previous_position = instance.depot.position;
    --i; // Re-evaluate this gene in the new route
  }

  routes.push_back(current_route);
  return routes;
}

auto genetic_algorithm(const Instance& instance, unsigned population_size, unsigned generations, double mutation_rate) -> Solution {
  auto population = initialize_population(instance, population_size);

  for (auto gen = 0u; gen < generations; ++gen) {
    auto new_population = Population{};

    while (new_population.size() < population_size) {
      auto [parent1, parent2] = select_parents(population);
      auto offspring = crossover(parent1, parent2);
      mutate(offspring, mutation_rate);
      offspring.fitness = evaluate_fitness(decode_individual(instance, offspring));
      new_population.push_back(offspring);
    }

    population = std::move(new_population);
  }

  const auto* best_individual = &population[0];
  for (const auto& individual : population) {
    if (individual.fitness < best_individual->fitness) {
      best_individual = &individual;
    }
  }

  auto best_routes = decode_individual(instance, *best_individual);
  return Solution{best_routes, best_individual->fitness};
}