#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "point.hpp"
#include <vector>
#include <string>

struct TimeWindow {
	double start = 0;
	double end = 0;
};

struct Depot {
	Point position{};
	TimeWindow time_window{};
};

struct Locker {
	unsigned id = 0;
	std::string name = "";
	Point position{};
	double capacity = 0;
};

struct Client {
	unsigned id = 0;
	std::string name = "";
	Point position{};
	TimeWindow time_window{};
	double demand = 0;
	std::vector<Locker> delivery_locations{};
};

struct Instance {
  Depot depot{};
  std::vector<Client> clients{};
  std::vector<Locker> lockers{};
  double vehicle_capacity = 0;
  int radius = 0;
};

auto read_instance(const std::string& filepath) -> Instance;
void defineDeliveryLocations(Instance&instance);
Client getClientById(Instance&instance, unsigned id);

#endif // INSTANCE_HPP