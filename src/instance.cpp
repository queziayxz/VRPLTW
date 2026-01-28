#include "instance.hpp"
#include "genetic.hpp"
#include "Split.hpp"
#include "LocalSearch.hpp"
#include "timer.hpp"
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <stdexcept>
#include <filesystem>

auto read_instance(const std::string& filepath) -> Instance {
//   namespace fs = std::filesystem;
// 	std::cout << "CWD = " << fs::current_path() << '\n';
// std::cout << "Tentando abrir: " << fs::absolute(p) << '\n';
	auto file = std::ifstream{filepath};
	if (!file) {
		throw std::runtime_error{"Erro em abrir o arquivo: "};
	}

	auto content = std::string{};

  auto num_clients = 0;
  auto num_lockers = 0;

	do {
		file >> content;
		if (content == "CUSTOMERS:") {
			file >> num_clients;
		}
		else if (content == "LOCKERS:") {
			file >> num_lockers;
		}
	}
	while (content != "CUSTOMER_SECTION");

	std::cout << "Numero de clientes: " << num_clients << '\n';
	std::cout << "Numero de lockers: " << num_lockers << '\n';

  	auto instance = Instance{};

	instance.vehicle_capacity = num_clients / 2.0;
	instance.radius = 15; // valor utilizado no artigo

	instance.clients.reserve((unsigned)num_clients);
	instance.lockers.reserve((unsigned)num_lockers);

	Client cliente_insertion{};

	for (auto i = 0u; i < (unsigned)num_clients; ++i) {
		auto id = 0u;
		file >> id;
		if (id != i + 1) {
			throw std::runtime_error("Erro na leitura dos dados do cliente.");
		}
		cliente_insertion.id = id;
		cliente_insertion.name =  std::string("C").append(std::to_string(id));
		file >> cliente_insertion.position.x >> cliente_insertion.position.y
				>> cliente_insertion.time_window.start >> cliente_insertion.time_window.end
				>> cliente_insertion.demand;

		instance.clients.emplace(id,cliente_insertion);
		
	}

	file >> content;
	if (content != "LOCKER_SECTION") {
    	throw std::runtime_error("Esperava encontrar LOCKER_SECTION, mas encontrou: " + content);
	}

	auto a = 0;
	file >> a >> instance.depot.position.x >> instance.depot.position.y
			>> instance.depot.time_window.start >> instance.depot.time_window.end >> a;

	Locker locker_insertion{};

	for (auto i = 0u; i < (unsigned)num_lockers; ++i) {
		auto id = 0u;
		file >> id;
		if (id != i + 1) {
			throw std::runtime_error{"Erro na leitura dos dados do locker."};
		}
		locker_insertion.id = id;
		locker_insertion.name = std::string("L").append(std::to_string(id));
		// instance.lockers[i].id = id;
		// instance.lockers[i].name = std::string("L").append(std::to_string(id));
		auto a = 0.0;
		file >> locker_insertion.position.x >> locker_insertion.position.y 
        >> a >> a >> locker_insertion.capacity;

		instance.lockers.emplace(id,locker_insertion);
	}

	std::cout << "leu os dados" << std::endl;
	
	defineDeliveryLocations(instance);
	std::cout << "adicionou delivary" << std::endl;

	// for(Client client : instance.clients) {
	// 	for(Locker locker : client.delivery_locations) {
	// 		std::cout << "client: " << client.id << std::endl;
	// 		std::cout << "locker: " << locker.position.x << "," << locker.position.y << std::endl;
			
	// 	}
	// }

	// std::cout << "Depot: " << instance.depot.position.x << ", " << instance.depot.position.y
    //         << " | Time Window: " << instance.depot.time_window.start << " - " << instance.depot.time_window.end << '\n';
//   for (const auto& client : instance.clients) {
//     std::cout << "Client " << client.name << ": " << client.position.x << ", " << client.position.y
//               << " | Time Window: " << client.time_window.start << " - " << client.time_window.end
//               << " | Demand: " << client.demand << '\n';
//   }
//   for (const auto& locker : instance.lockers) {
//     std::cout << "Locker " << locker.name << ": " << locker.position.x << ", " << locker.position.y
//               << " | Capacity: " << locker.capacity << '\n';
//   }

  return instance;
}

void defineDeliveryLocations(Instance&instance)
{
	for (auto client = instance.clients.begin(); client != instance.clients.end(); ++client) {
		for(auto locker = instance.lockers.begin(); locker != instance.lockers.end(); ++locker) {
			if(distance(client->second.position,locker->second.position) <= instance.radius) {
				client->second.delivery_locations.push_back(locker->second);
			}
		}
	}
}

void resultOnlyInstance(std::string instanceName)
{
	// namespace fs = std::filesystem;

	// fs::path src_dir = fs::path(__FILE__).parent_path();
  	// std::cout << "caminho instance: " << src_dir << std::endl;
	std::cout << "CWD = " << std::filesystem::current_path() << '\n';

	try {
		std::string path = ("instances/");
		std::string totalPath = path.append(instanceName);
		std::cout << "path: " << totalPath << std::endl;
		auto instance = read_instance(totalPath);
		auto timer = Timer{};
    	auto solution = genetic_algorithm(instance, 60, 100, 0.05);
		auto elapsed_time = timer.elapsed();
	
		std::cout << "Total da distancia: " << solution.total_distance << '\n';
    	std::cout << "Numero de rotas: " << solution.routes.size() << '\n';
    	std::cout << "Tempo total (ms): " << elapsed_time << '\n';

		for(auto i = 0u; i < solution.routes.size(); i++) {
			std::cout << "Rota (" << solution.routes[i].total_distance << ")(" << solution.routes[i].load << "): ";
			for(auto j = 0u; j < solution.routes[i].customers.size(); j++) {
				if(solution.routes[i].assigned_lockers[j] == -1) {
				std::cout << "C" << solution.routes[i].customers[j] << " - ";
				continue;
				}
				int locker = solution.routes[i].assigned_lockers[j];
				std::cout << "L" << locker;
				std::cout << "(";
				while(solution.routes[i].assigned_lockers[j] == locker) {
				std::cout << solution.routes[i].customers[j] << ",";
				j++;
				}
				std::cout << ") - ";
				j--;
			}
			std::cout << std::endl;
    	}

	} catch(std::runtime_error&e) {
		std::cout << e.what() << std::endl;
	}
}

void resultAllInstances()
{
	constexpr auto instances = std::array{
		"n20w100l2_1",
		"n20w100l2_2",
		"n20w100l2_3",
		"n20w100l2_4",
		"n20w100l2_5",
		"n20w20l2_1",
		"n20w20l2_2",
		"n20w20l2_3",
		"n20w20l2_4",
		"n20w20l2_5",
		"n40w100l4_1",
		"n40w100l4_2",
		"n40w100l4_3",
		"n40w100l4_4",
		"n40w100l4_5",
		"n40w20l4_1",
		"n40w20l4_2",
		"n40w20l4_3",
		"n40w20l4_4",
		"n40w20l4_5",
		"n60w100l6_1",
		"n60w100l6_2",
		"n60w100l6_3",
		"n60w100l6_4",
		"n60w100l6_5",
		"n60w20l6_1",
		"n60w20l6_2",
		"n60w20l6_3",
		"n60w20l6_4",
		"n60w20l6_5",
		"n100w100l10_1",
		"n100w100l10_2",
		"n100w100l10_3",
		"n100w100l10_4",
		"n100w100l10_5",
		"n100w20l10_1",
		"n100w20l10_2",
		"n100w20l10_3",
		"n100w20l10_5"
  };

  auto instance_prefix = std::string{"instances/"};
  constexpr auto instance_suffix = ".vrpl";

  auto results = std::ofstream{"../results.txt"};

  results << "Instance BestCost BestTime(s) AvgCost AvgTime(s)\n";

  for (auto instance_id = 1; unsigned(instance_id) <= instances.size(); ++instance_id) {
		const auto* instance_name = instances[unsigned(instance_id) - 1];

		std::cout << "> Running instance: " << instance_name << " (" << instance_id << '/' << instances.size() << ")\n";

		auto total_time = 0.0;
		auto total_cost = 0.0;
		auto best_time = 0.0;
		auto best_cost = std::numeric_limits<double>::max();

		constexpr auto runs = 5;

		auto instance = read_instance(instance_prefix + instance_name + instance_suffix);

		for (auto i = 0; i < runs; ++i) {
			auto timer = Timer{};
			auto solution = genetic_algorithm(instance, 60, 100, 0.05);
			auto elapsed_time = timer.elapsed() / 1000.0;

			total_time += elapsed_time;
			total_cost += solution.total_distance;
			
			if (solution.total_distance < best_cost) {
				best_cost = solution.total_distance;
				best_time = elapsed_time;
			}
		}

		results << instance_name << " "
				<< best_cost << " "
				<< best_time << " "
				<< (total_cost / runs) << " "
				<< (total_time / runs) << '\n';
  	}
}