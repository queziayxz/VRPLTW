#ifndef SPLIT_HPP
#define SPLIT_HPP

#include <vector>
#include <deque>
#include <optional>
#include <unordered_set>

#include "instance.hpp"
#include "point.hpp"
#include "genetic.hpp"

struct DataRoute {
    std::vector<int> route{};                           // ids positivos sao clientes, negativos sao lockers
    double distance = 0.0;
    bool isValid = true;
    std::unordered_set<int> lockers;              // guarda os ids dos locker utilizados naquela rota
};

class Split {
    private:
        Individual individual{};                        // cromossomo passado onde ira conter a giant tour
        Instance instance{};                            // atributo da instancia atual
        Depot depot{};                                  // deposito
        std::vector<double> cumulative_distance{};      // distancia acumulada de cada rota
        std::vector<double> cumulative_timeWindow{};    // time window acumulada de cada rota
        std::vector<int> cumulative_demand{};           // demanda acumulada de cada veiculo
        std::vector<int> cumulative_capacity_locker{};  // capacidade acumulada de cada locker
        std::vector<int> pred{};                        // predecessores da rota otima

        std::deque<int> deque{};                        // deque dos melhores clientes

        int front = 0;
        int back = 0;

    public:
        Split(Instance instance, Individual individual);
        void splitLinear();

        bool dominates(int back, int prev, int t, int capacity);
        int indexDominates(int a, int b, int capacity);
        std::optional<DataRoute> calcDistanceRoute(int i, int j);

        bool verifyBestLocker(Locker&locker, Point&prev, double cumulative_cost, double cumulativeDistance, std::unordered_set<int>&lockersUsed);
        bool verifyLocationLocker(Locker&locker, Client&clientB, std::unordered_set<int>&lockersUsed);
        std::optional<Locker> getBestLockerDistance(Client&client, std::unordered_set<int>&lockersUsed);
        std::optional<Locker> getLockerCommon(Client&clientA, Client&clientB, std::unordered_set<int>&lockersUsed);
};

#endif