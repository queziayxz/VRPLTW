#include "genetic.hpp"
#include "random.hpp"
#include "Split.hpp"
#include "LocalSearch.hpp"
#include <numeric>
#include <algorithm>
#include <limits>
#include <optional>
#include <queue>

#include <iostream>

const double INF = std::numeric_limits<double>::infinity();

auto random_individual(Instance& instance) -> Individual {
  Split split(instance);
  auto individual = Individual{};
  individual.chromosome = std::vector<int>(instance.clients.size());
  std::iota(individual.chromosome.begin(), individual.chromosome.end(), 1); // inicializa com valores dos indices
  std::shuffle(individual.chromosome.begin(), individual.chromosome.end(), Random::mt); // embaralha as posicoes do vetor
  return individual;
}

auto initialize_population(Instance& instance, unsigned population_size) -> Population 
{
  auto population = Population{};
  Split split(instance);

  for (auto i = 0u; i < population_size; ++i) {
    population.push_back(random_individual(instance));
    population.back().fitness = evaluate_fitness(split.splitLinear(population.back()),instance);
    // std::cout << "parou no spliut" << std::endl;
  }

  return population;
}

auto select_parents(const Population& population) -> std::pair<const Individual&, const Individual&> {
  // Tournament selection
  auto tournament_size = 5u; // faz o terneio 5 vezes
  const auto* best1 = &population[Random::get_int(0u, (unsigned)population.size() - 1u)]; //seleciona um individuo randomicamente da populacao
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
    if (Random::get_real(0.0, 1.0) < mutation_rate) { // calcula a probabilidade de realizar a mutacao
      auto j = Random::get_int(0u, (unsigned)individual.chromosome.size() - 1u);
      std::swap(individual.chromosome[i], individual.chromosome[j]); // swap na giant tour
    }
  }
}

auto evaluate_fitness(const std::vector<Route>& routes, Instance&instance) -> double {
  auto total_distance = 0.0;

  for (int i = 0; i < routes.size(); i++) {
    total_distance += routes[i].total_distance;
    for(int j = 0; j < routes[i].customers.size(); j++) {
      if(routes[i].assigned_lockers[j] > 0) {
        total_distance += ::distance(instance.clients.at(routes[i].customers[j]).position,instance.lockers.at(routes[i].assigned_lockers[j]).position);
      }
    }
  }
  return total_distance;
}

auto can_serve_client(double vehicle_capacity, const Point& prev_position, const Client& client, const Depot& depot,
  double current_time, double current_load, double& new_time
) -> bool {
  if (current_load + client.demand > vehicle_capacity) {
    std::cout << "caiu demanda ultrapassa" << std::endl;
    return false;
  }
  
  auto arrival = current_time + distance(prev_position, client.position); //lembrando de distancia da rota eh igual ao tempo de viagem
  auto start = std::max(arrival, client.time_window.start);
  
  if (start > client.time_window.end) {
    std::cout << "caiu time window" << std::endl;
    std::cout << "curren_time: " << current_time << std::endl;
    std::cout << "prev_position,x: " << prev_position.x << std::endl;
    std::cout << "prev_position.y: " << prev_position.y << std::endl;
    std::cout << "client.position.x: " << client.position.x << std::endl;
    std::cout << "client.position.y: " << client.position.y << std::endl;
    std::cout << "distance: " << distance(prev_position, client.position) << std::endl;
    std::cout << "start: " << start << std::endl;
    std::cout << "client.time_window.end: " << client.time_window.end << std::endl;
    return false;
  }
  
  if (start + distance(client.position, depot.position) > depot.time_window.end) {
    std::cout << "caiu time window 2" << std::endl;
    return false;
  }

  new_time = start;
  return true;
}

auto try_locker(double vehicle_capacity, const Point& prev_position, const Client& client, 
  const std::vector<double>& locker_remaining_capacities, const std::vector<Locker>& lockers,
  double current_time, double current_load, double& new_time
) -> int {
  if (current_load + client.demand > vehicle_capacity) {
    return -1;
  }

  auto best_locker = -1;
  auto best_cost = std::numeric_limits<double>::max();

  for (Locker locker : lockers) {
    if (locker_remaining_capacities[locker.id-1] < client.demand) continue;

    if(current_load+client.demand > locker_remaining_capacities[locker.id-1]) continue;

    auto cost = distance(prev_position, locker.position);
    double cost_locker_client = distance(locker.position,client.position);
    double cost_prev_client = distance(prev_position,client.position);

    if(cost+cost_locker_client > cost_prev_client) {
      // std::cout << "nao vantajoso locker: " << locker.id << std::endl;
      // std::cout << "size lockers: " << lockers.size() << std::endl;
      // std::cout << "cost: " << cost << std::endl;
      // std::cout << "cost_locker_client: " << cost_locker_client << std::endl;
      // std::cout << "cost_prev_client: " << cost_prev_client << std::endl;
      continue;
    }
    
    // std::cout << "nao deu continue locker: " << locker.id << std::endl;
    // std::cout << "cliente: " << client.id << std::endl;
    // std::cout << "cost: " << cost << std::endl;
    // std::cout << "cost_locker_client: " << cost_locker_client << std::endl;
    // std::cout << "cost_prev_client: " << cost_prev_client << std::endl;


    if (cost >= best_cost) {
      continue;
    }

    auto arrival = current_time + cost;

    // deposito nem locker tem janela de tempo
    // if (arrival + distance(locker.position, depot.position) > depot.time_window.end) {
    //   continue;
    // }

    best_locker = (int)(locker.id-1);
    best_cost = cost;
    new_time = arrival;
  }

  return best_locker;
}

// auto decode_individual(Instance& instance, const Individual& individual) -> std::vector<Route> {
//   // Split the chromosome into routes based on vehicle capacity, time windows, and locker assignments
//   auto routes = std::vector<Route>{};
//   std::queue<int> clients;

//   auto locker_remaining_capacities = std::vector<double>(instance.lockers.size());
//   for (auto i = 0u; i < instance.lockers.size(); ++i) {
//     locker_remaining_capacities[i] = instance.lockers[i].capacity;
//   }

//   // for(Client cliente : instance.customers) {
//   //   clients.push(client.id);
//   // }

//   auto current_route = Route{};
//   auto current_time = instance.depot.time_window.start; // pega o tempo atual da rota
//   auto previous_position = instance.depot.position;     // posicao atual na rota
  
//   std::vector<Locker> delivery_locations{};             // vetor dos lockers possiveis para o cliente

//   size_t i = 0;
//   while(i < individual.chromosome.size()) {
//     std::cout << "num clientes: " << individual.chromosome.size() << std::endl;
//     int gene = individual.chromosome[i];
//     std::cout << "pegoou: " << i << std::endl;
//     std::cout << "gene: " << gene << std::endl;
//     const auto& client = instance.clients.at(gene);
//     bool add = false;
    
//     delivery_locations = client.delivery_locations;
//     std::cout << "client: " << client.id << std::endl;

//     auto locker = try_locker(instance.vehicle_capacity, previous_position, client,
//         locker_remaining_capacities, delivery_locations, current_time, current_route.load, current_time);

//     if (locker > -1) {
//       std::cout << "locker bom: " << locker << std::endl;
//       current_route.customers.push_back(gene);
//       current_route.assigned_lockers.push_back(locker);
//       current_route.load += client.demand;
//       current_route.total_distance += distance(previous_position, instance.lockers[(unsigned)locker].position);
//       previous_position = instance.lockers[(unsigned)locker].position;
//       locker_remaining_capacities[(unsigned)locker] -= client.demand;
//       std::cout << "adicionou locker: " << std::endl;
//       add = true;
//     } else if (can_serve_client(instance.vehicle_capacity, previous_position, client, instance.depot, current_time, current_route.load, current_time)) {
//       std::cout << "chamou cliente: " << std::endl;
//       current_route.customers.push_back(gene); // adiciona cliente na rota
//       current_route.assigned_lockers.push_back(-1); // coloca -1 pra informar que nao usou esse locker
//       current_route.load += client.demand; // adiciona a demanda do cliente no veiculo
//       current_route.total_distance += distance(previous_position, client.position); 
//       previous_position = client.position; // seta a posicao atual
//       std::cout << "adicionou cliente: " << std::endl;
//       add = true;
//     }

//     if(add) {
//       i++;
//     } else {
//       std::cout << "nao adicionou em nehuma rota cliente: " << client.id << std::endl;
//       std::cout << "clientes faltantes: " << individual.chromosome.size()-1 - i << std::endl;
      
//       // se chegar até aqui, nem o locker ou veiculo nao tem capacidade 
//       // para adicionar essa demanda ou erro na janela de tempo do cliente
//       routes.push_back(current_route);
  
//       current_route = Route{};
//       current_time = instance.depot.time_window.start;
//       previous_position = instance.depot.position;
  
//       std::cout << "quant rootas: " << routes.size() << std::endl;
//     }


//   }
  
//   std::cout << "terminou " << std::endl;
//   routes.push_back(current_route);
//   return routes;
// }

auto genetic_algorithm(Instance& instance, unsigned population_size, unsigned generations, double mutation_rate) -> Solution 
{
  auto population = initialize_population(instance, population_size); //vertor de individuos
  
  // std::cout << "criou população inicial" << std::endl;
  Split split(instance);
  LocalSearch localSearch(instance);

  for (auto gen = 0u; gen < generations; ++gen) {
    auto new_population = Population{};

    while (new_population.size() < population_size) {
      auto [parent1, parent2] = select_parents(population);
      auto offspring = crossover(parent1, parent2);
      // offspring = localSearch.iteratedGreedy(offspring,5);
      offspring.fitness = evaluate_fitness(split.splitLinear(offspring),instance);
      new_population.push_back(offspring);
    }

    population = std::move(new_population);
  }

  auto* best_individual = &population[0];
  for (auto& individual : population) {
    if (individual.fitness < best_individual->fitness) {
      best_individual = &individual;
    }
  }

  // std::cout << "giant tour final: " << std::endl;
  // for(int i = 0; i < best_individual->chromosome.size(); i++) {
  //   std::cout << best_individual->chromosome[i] << " ";
  // }
  // std::cout << std::endl;
  auto best_routes = split.splitLinear(*best_individual);
  return Solution{best_routes, best_individual->fitness};
}