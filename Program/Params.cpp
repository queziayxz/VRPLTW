#include "Params.h"

// The universal constructor for both executable and shared library
// When the executable is run from the commandline,
// it will first generate an CVRPLIB instance from .vrp file, then supply necessary information.
Params::Params(const std::vector<node>& nodes,
	const std::vector<std::vector<double>>& distances,
	int nbClients,
	int nbLockers,
	double vehicleCapacity,
	int nbVeh,
	bool verbose,
	const AlgorithmParameters& ap)
	: ap(ap), nbVehicles(nbVeh), durationLimit(durationLimit), nbClients(nbClients), nbLockers(nbLockers),
	  vehicleCapacity(vehicleCapacity), timeCost(distances), verbose(verbose)
{
	// This marks the starting time of the algorithm
	startTime = clock();

	totalDemand = 0.;
	maxDemand = 0.;

	// Initialize RNG
	ran.seed(ap.seed);

	// check if valid coordinates are provided
	areCoordinatesProvided = true;

	cli = nodes;
	for (int i = 1; i < nbClients + nbLockers + 1; i++)
	{
		// If useSwapStar==false, x_coords and y_coords may be empty.
		if (ap.useSwapStar == 1 && areCoordinatesProvided) {
			cli[i].polarAngle = CircleSector::positive_mod(
				32768. * atan2(cli[i].y - cli[0].y, cli[i].x - cli[0].x) / PI);
		}
		else {
			cli[i].x = 0.0;
			cli[i].y = 0.0;
			cli[i].polarAngle = 0.0;
		}
	}

	for (int i = 1; i < nbClients + 1; ++i) {
		if (cli[i].demand > maxDemand) maxDemand = cli[i].demand;
		totalDemand += cli[i].demand;
	}

	if (verbose && ap.useSwapStar == 1 && !areCoordinatesProvided)
		std::cout << "----- NO COORDINATES HAVE BEEN PROVIDED, SWAP* NEIGHBORHOOD WILL BE DEACTIVATED BY DEFAULT" << std::endl;

	// Default initialization if the number of vehicles has not been provided by the user
	if (nbVehicles == INT_MAX)
	{
		nbVehicles = (int)std::ceil(1.3*totalDemand/vehicleCapacity) + 3;  // Safety margin: 30% + 3 more vehicles than the trivial bin packing LB
		if (verbose) 
			std::cout << "----- FLEET SIZE WAS NOT SPECIFIED: DEFAULT INITIALIZATION TO " << nbVehicles << " VEHICLES" << std::endl;
	}
	else
	{
		if (verbose)
			std::cout << "----- FLEET SIZE SPECIFIED: SET TO " << nbVehicles << " VEHICLES" << std::endl;
	}

	// Calculation of the maximum distance
	maxDist = 0.;
	for (int i = 0; i <= nbClients + nbLockers; i++)
		for (int j = 0; j <= nbClients + nbLockers; j++)
			if (timeCost[i][j] > maxDist) maxDist = timeCost[i][j];

	// Calculation of the correlated vertices for each customer (for the granular restriction)
	correlatedVertices = std::vector<std::vector<int> >(nbClients + nbLockers + 1);
	std::vector<std::set<int> > setCorrelatedVertices = std::vector<std::set<int> >(nbClients + nbLockers + 1);
	std::vector<std::pair<double, int> > orderProximity;
	for (int i = 1; i <= nbClients + nbLockers; i++)
	{
		orderProximity.clear();
		for (int j = 1; j <= nbClients + nbLockers; j++)
			if (i != j) orderProximity.emplace_back(timeCost[i][j], j);
		std::sort(orderProximity.begin(), orderProximity.end());

		for (int j = 0; j < std::min<int>(ap.nbGranular, nbClients - 1); j++)
		{
			// If i is correlated with j, then j should be correlated with i
			setCorrelatedVertices[i].insert(orderProximity[j].second);
			setCorrelatedVertices[orderProximity[j].second].insert(i);
		}
	}

	// Filling the vector of correlated vertices
	for (int i = 1; i <= nbClients + nbLockers; i++)
		for (int x : setCorrelatedVertices[i])
			correlatedVertices[i].push_back(x);

	// Safeguards to avoid possible numerical instability in case of instances containing arbitrarily small or large numerical values
	if (maxDist < 0.1 || maxDist > 100000)
		throw std::string(
			"The distances are of very small or large scale. This could impact numerical stability. Please rescale the dataset and run again.");
	if (maxDemand < 0.1 || maxDemand > 100000)
		throw std::string(
			"The demand quantities are of very small or large scale. This could impact numerical stability. Please rescale the dataset and run again.");
	if (nbVehicles < std::ceil(totalDemand / vehicleCapacity))
		throw std::string("Fleet size is insufficient to service the considered clients.");

	// A reasonable scale for the initial values of the penalties
	penaltyDuration = 1;
	penaltyCapacity = std::max<double>(0.1, std::min<double>(1000., maxDist / maxDemand));

	std::cout << "----- INSTANCE SUCCESSFULLY LOADED WITH " << nbClients << " CLIENTS, " << nbLockers << " LOCKERS AND " << nbVehicles << " VEHICLES" << std::endl;
}


