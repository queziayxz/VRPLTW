#include "instance.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

auto read_instance(const std::string& filepath) -> Instance {
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

	instance.clients = std::vector<Client>((unsigned)num_clients);
	instance.lockers = std::vector<Locker>((unsigned)num_lockers);

	for (auto i = 0u; i < (unsigned)num_clients; ++i) {
		auto id = 0u;
		file >> id;
		if (id != i + 1) {
			throw std::runtime_error("Erro na leitura dos dados do cliente.");
		}
		instance.clients[i].id = id;
		instance.clients[i].name = std::string("C").append(std::to_string(id));
		file >> instance.clients[i].position.x >> instance.clients[i].position.y
				>> instance.clients[i].time_window.start >> instance.clients[i].time_window.end
				>> instance.clients[i].demand;
	}

	file >> content;
	if (content != "LOCKER_SECTION") {
    	throw std::runtime_error("Esperava encontrar LOCKER_SECTION, mas encontrou: " + content);
	}

	auto a = 0;
	file >> a >> instance.depot.position.x >> instance.depot.position.y
			>> instance.depot.time_window.start >> instance.depot.time_window.end >> a;

	for (auto i = 0u; i < (unsigned)num_lockers; ++i) {
		auto id = 0u;
		file >> id;
		if (id != i + 1) {
			throw std::runtime_error{"Erro na leitura dos dados do locker."};
		}
		instance.lockers[i].id = id;
		instance.lockers[i].name = std::string("L").append(std::to_string(id));
		auto a = 0.0;
		file >> instance.lockers[i].position.x >> instance.lockers[i].position.y 
        >> a >> a >> instance.lockers[i].capacity;
	}

	defineDeliveryLocations(instance);

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
	for(Client&client : instance.clients) {
		for(Locker&locker : instance.lockers) {
			if(distance(client.position,locker.position) <= instance.radius) {
				client.delivery_locations.push_back(locker);
			}
		}
	}
}

Client getClientById(Instance&instance, unsigned id)
{
	Client clientReturn;
	for(Client client : instance.clients) {
		if(client.id == id+1){
			clientReturn = client;
			break;
		} 
	}

	return clientReturn;
}

Client getClientById_2(Instance&instance, unsigned id)
{
	Client clientReturn;
	for(Client client : instance.clients) {
		if(client.id == id){
			clientReturn = client;
			break;
		} 
	}

	return clientReturn;
}