#include "instance.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

auto read_instance(const std::string& filepath) -> Instance {
  auto file = std::ifstream{filepath};
	if (!file) {
		throw std::runtime_error{"Could not open file: "};
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

	std::cout << "Number of clients: " << num_clients << '\n';
	std::cout << "Number of lockers: " << num_lockers << '\n';

  auto instance = Instance{};

	instance.vehicle_capacity = num_clients / 2.0;

	instance.clients = std::vector<Client>((unsigned)num_clients);
	instance.lockers = std::vector<Locker>((unsigned)num_lockers);

	for (auto i = 0u; i < (unsigned)num_clients; ++i) {
		auto id = 0u;
		file >> id;
		if (id != i + 1) {
			throw std::runtime_error("Error while reading client data.");
		}
		file >> instance.clients[i].position.x >> instance.clients[i].position.y
				>> instance.clients[i].time_window.start >> instance.clients[i].time_window.end
				>> instance.clients[i].demand;
	}

	file >> content;
	if (content != "LOCKER_SECTION") {
    throw std::runtime_error("Expected LOCKER_SECTION, found: " + content);
	}

	auto a = 0;
	file >> a >> instance.depot.position.x >> instance.depot.position.y
			>> instance.depot.time_window.start >> instance.depot.time_window.end >> a;

	for (auto i = 0u; i < (unsigned)num_lockers; ++i) {
		auto id = 0u;
		file >> id;
		if (id != i + 1) {
			throw std::runtime_error{"Error while reading locker data."};
		}
		auto a = 0.0;
		file >> instance.lockers[i].position.x >> instance.lockers[i].position.y 
        >> a >> a >> instance.lockers[i].capacity;
	}

	// std::cout << "Depot: " << instance.depot.position.x << ", " << instance.depot.position.y
  //           << " | Time Window: " << instance.depot.time_window.start << " - " << instance.depot.time_window.end << '\n';
  // for (const auto& client : instance.clients) {
  //   std::cout << "Client: " << client.position.x << ", " << client.position.y
  //             << " | Time Window: " << client.time_window.start << " - " << client.time_window.end
  //             << " | Demand: " << client.demand << '\n';
  // }
  // for (const auto& locker : instance.lockers) {
  //   std::cout << "Locker: " << locker.position.x << ", " << locker.position.y
  //             << " | Capacity: " << locker.capacity << '\n';
  // }

  return instance;
}