#ifndef SPLIT_HPP
#define SPLIT_HPP

#include <vector>
#include <deque>

#include "instance.hpp"
#include "genetic.hpp"

class Split {
    private:
        Individual individual{};                  // cromossomo passado onde ira conter a giant tour
        Instance instance{};                      // atributo da instancia atual
        Depot depot{};                            // deposito
        std::vector<double> cumulative_distance{};   // distancia acumulada de cada rota
        std::vector<int> cumulative_demand{};   // distancia acumulada de cada rota
        std::vector<int> pred{};                  // predecessores da rota otima

        std::deque<int> deque{};                    // deque dos melhores clientes

        int front = 0;
        int back = 0;

    public:
        Split(Instance instance, Individual individual);
        void splitLinear();

        bool dominates(int back, int prev, int t, int capacity);
        int indexDominates(int a, int b, int capacity);
        double calcDistanceRoute(int i, int j);
};

#endif