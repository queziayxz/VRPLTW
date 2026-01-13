//
// Created by chkwon on 3/22/22.
//

#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include "InstanceCVRPLIB.h"

InstanceCVRPLIB::InstanceCVRPLIB(std::string pathToInstance)
{
	std::ifstream inputFile(pathToInstance);
	if (!inputFile.is_open()) {
		throw std::string("Impossible to open instance file: " + pathToInstance);
	}

	std::string content{};

	do {
		inputFile >> content;
		if (content == "CUSTOMERS:") {
			inputFile >> nbClients;
		}
		else if (content == "LOCKERS:") {
			inputFile >> nbLockers;
		}
	}
	while (content != "CUSTOMER_SECTION");

	std::cout << "Number of clients: " << nbClients << std::endl;
	std::cout << "Number of lockers: " << nbLockers << std::endl;
	vehicleCapacity = nbClients / 2.0;

	nodes = std::vector<node>(nbClients + nbLockers + 1); // +1 for depot

	auto offset = 0;
	while (std::getline(inputFile, content)) {
		if (content == "LOCKER_SECTION") {
			offset = nbClients;
			continue;
		}
		
		auto sstream = std::stringstream{content};
		auto id = 0;
		sstream >> id;
		id += id == 0 ? 0 : offset;
		sstream >> nodes[id].x >> nodes[id].y
						>> nodes[id].tw.start >> nodes[id].tw.end
						>> nodes[id].demand;
	}

	//print each node
	// for (int i = 0; i < nodes.size(); i++) {
	// 	std::cout << "Node " << i << ": (" << nodes[i].x << ", " << nodes[i].y << "), TW: [" << nodes[i].tw.start << ", " << nodes[i].tw.end << "], Demand: " << nodes[i].demand << std::endl;
	// }

	// Calculating 2D Euclidean Distance
	auto num_nodes = nodes.size();
	distances = std::vector<std::vector<double>>(num_nodes, std::vector<double>(num_nodes));
	for (int i = 0; i < num_nodes; i++) {
		for (int j = 0; j < num_nodes; j++) {
			auto& a = nodes[i];
			auto& b = nodes[j];
			auto dx = a.x - b.x;
			auto dy = a.y - b.y;
			distances[i][j] = std::sqrt(dx * dx + dy * dy);
		}
	}
}
