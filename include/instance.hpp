#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "point.hpp"
#include <vector>
#include <string>

struct TimeWindow {
	double start;
	double end;
};

struct Depot {
	Point position;
	TimeWindow time_window;
};

struct Client {
	Point position;
	TimeWindow time_window;
	double demand;
};

struct Locker {
	Point position;
	double capacity;
};

struct Instance {
  Depot depot;
  std::vector<Client> clients;
  std::vector<Locker> lockers;
  double vehicle_capacity;
};

auto read_instance(const std::string& filepath) -> Instance;

#endif // INSTANCE_HPP