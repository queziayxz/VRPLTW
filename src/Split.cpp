#include <iostream>

#include "Split.hpp"
// #include "point.hpp"

Split::Split(Instance instance, Individual individual)
{
    this->instance = instance;
    this->individual = individual;
    this->depot = instance.depot;

    this->front = 0;
    this->back = 0;
    this->cumulative_distance = std::vector<double>(individual.chromosome.size()+1);
    this->cumulative_demand = std::vector<int>(individual.chromosome.size()+1);
    this->pred = std::vector<int>(individual.chromosome.size()+1);
}

void Split::splitLinear()
{
    this->cumulative_distance[0] = 0.0;
    this->deque.clear();

    this->deque.push_back(0);

    int index = 0;
    this->cumulative_demand[0] = 0;
    for(int i = 1; i < (int)this->cumulative_demand.size(); i++) {
        int indexPass = this->individual.chromosome[(unsigned)i-1];
        index = this->individual.chromosome[(unsigned)i];
        this->cumulative_demand[(unsigned)i] = this->cumulative_demand[(unsigned)i-1] + (int)getClientById(this->instance,(unsigned)indexPass).demand;
        
        this->cumulative_distance[(unsigned)i] = this->cumulative_distance[(unsigned)i-1] + 
            distance(getClientById(this->instance,(unsigned)indexPass).position,getClientById(this->instance,(unsigned)index).position);
    }

    std::cout << "demandas acumuladas" << std::endl;
    for(int i = 0; i < (int)this->cumulative_demand.size(); i++) {
        std::cout << this->cumulative_demand[(unsigned)i] << " ";
    }
    
    std::cout << std::endl;
    
    for(int t = 1; t <= static_cast<int>(this->individual.chromosome.size()); t++) {
        this->cumulative_distance[(unsigned)t] = this->cumulative_distance[(unsigned)this->deque.front()] + (int)calcDistanceRoute(this->deque.front(),t);
        this->pred[(unsigned)t] = this->deque.front();

        if(t < static_cast<int>(this->individual.chromosome.size())) {
            if(this->deque.size() > 2) {
                while(dominates(this->deque.back(),this->deque[this->deque.size()-2],t,5)) {
                    // std::cout << "cliente " << t << " domina " << this->deque.back() << std::endl;
                    this->deque.pop_back();
                }
            }
            std::cout << "this->deque.front(): " << this->deque.front() << std::endl;
            std::cout << "this->cumulative_demand[(unsigned)this->deque.front()]: " << this->cumulative_demand[(unsigned)this->deque.front()] << std::endl;
            std::cout << "this->cumulative_demand[(unsigned)t+1]: " << this->cumulative_demand[(unsigned)t+1] << std::endl;
            std::cout << "this->instance.vehicle_capacity: " << this->instance.vehicle_capacity << std::endl;
            this->deque.push_back(t);
            while(this->cumulative_demand[(unsigned)t+1] > this->instance.vehicle_capacity + this->cumulative_demand[(unsigned)this->deque.front()]) {
                std::cout << "entrou falta capacidade: " << std::endl;
                this->deque.pop_front();
                std::cout << "novo front: " << this->deque.front() << std::endl;
                std::cout << "this->cumulative_demand[(unsigned)this->deque.front()]: " << this->cumulative_demand[(unsigned)this->deque.front()] << std::endl;
                std::cout << "this->cumulative_demand[(unsigned)t+1]: " << this->cumulative_demand[(unsigned)t+1] << std::endl;
                std::cout << "this->instance.vehicle_capacity: " << this->instance.vehicle_capacity << std::endl;
            }
        }
    }

    for(int i = 1; i < (int)this->pred.size(); i++) {
        std::cout << this->pred[(unsigned)i] << " ";
    }

    std::cout << std::endl;
    
    int end = (int)this->pred.size()-1;
    std::cout << "end: " << end << std::endl; // ultimo predecessor adicionado
    int j = 1;
    while(end > 1) {
        int init = (int)this->pred[(unsigned)end];
        std::cout << "Rota: " << j << std::endl;
        for(int i = init+1; i <= end; i++) {
            std::cout << this->individual.chromosome[(unsigned)i-1] << " ";
        }
        end = init;
        j++;
        std::cout << std::endl;
    }
    // std::cout << "f(0,2): " << calcDistanceRoute(0,2) << std::endl; // 0,1
}

bool Split::dominates(int back, int prev, int t, int capacity)
{
    if(this->deque.size() < 2) return false; // nao possui elementos suficientes para verificar a dominacao

    int indexBack = indexDominates(prev,back,capacity);
    int indexT = indexDominates(back,t,capacity);

    std::cout << "prev " << prev << std::endl;
    std::cout << "back " << back << std::endl;
    std::cout << "t " << t << std::endl;
    std::cout << "indexBack " << indexBack << std::endl;
    std::cout << "indexT " << indexT << std::endl;

    if(indexT <= indexBack) return true;

    return false;
}

int Split::indexDominates(int a, int b, int capacity)
{
    int low = b;
    int hight = std::min((int)this->individual.chromosome.size()-1, b+capacity);
    int mid = 0;
    int best = hight + 1; // soma mais um no limite para caso retorne esse valor, ele ser menor na comparação do dominates

    while(low < hight) {
        mid = (low+hight)/2; // calcula ponto medio do intervalo
        double costA = this->cumulative_distance[(unsigned)a] + calcDistanceRoute(a,mid);
        double costB = this->cumulative_distance[(unsigned)b] + calcDistanceRoute(b,mid);

        if(costB < costA) { // B ja eh melhor que A nesse intervalor
            best = mid;
            hight = mid - 1;
        } else { // B ainda nao eh melhor que A no intervalo
            low = mid + 1; 
        }
    }

    return best;
}

double Split::calcDistanceRoute(int i, int j)
{
    j = j > 0 ? j-1 : j;
    // int index = this->individual.chromosome[(unsigned)i];
    // double distance = ::distance(this->depot.position, getClientById(this->instance,(unsigned)index).position);
    int index = 0;
    double distance = 0;
    // std::cout << "dist 1: " << distance << std::endl;
    for(int x = i; x < j; x++) {
        index = this->individual.chromosome[(unsigned)x];
        distance += ::distance(getClientById(this->instance,(unsigned)index).position, getClientById(this->instance,(unsigned)index+1).position);
        // std::cout << "dist 2: " << distance << std::endl;
    }
    
    // index = this->individual.chromosome[(unsigned)j];
    // distance += ::distance(getClientById(this->instance,(unsigned)index).position, this->depot.position);
    // std::cout << "dist 3: " << distance << std::endl;

    return distance;
}