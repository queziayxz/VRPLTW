//
// Created by chkwon on 3/22/22.
//

#ifndef INSTANCECVRPLIB_H
#define INSTANCECVRPLIB_H
#include<string>
#include<vector>

struct time_window {
	double start{};
	double end{};
};

struct node {
	double x{};
	double y{};
	time_window tw{};
	double demand{};
	int polarAngle{};
};

class InstanceCVRPLIB
{
public:
	std::vector<node> nodes; // depot, clients and lockers
	std::vector<std::vector<double>> distances;
	double vehicleCapacity;							// Capacity limit
	int nbClients;											// Number of clients (excluding the depot)
	int nbLockers;

	InstanceCVRPLIB(std::string pathToInstance);
};


#endif //INSTANCECVRPLIB_H
