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
    this->cumulative_timeWindow = std::vector<double>(individual.chromosome.size()+1);
    this->cumulative_demand = std::vector<int>(individual.chromosome.size()+1);
    this->pred = std::vector<int>(individual.chromosome.size()+1);
}

void Split::splitLinear()
{
    this->cumulative_distance[0] = 0.0;
    this->cumulative_demand[0] = 0;
    this->cumulative_timeWindow[0] = 0.0;
    this->deque.clear();

    this->deque.push_back(0);

    // int index = 0;
    for(int i = 1; i < (int)this->cumulative_demand.size(); i++) {
        int indexPass = this->individual.chromosome[(unsigned)i-1];
        // index = this->individual.chromosome[(unsigned)i];
        this->cumulative_demand[(unsigned)i] = this->cumulative_demand[(unsigned)i-1] + (int)getClientById(this->instance,(unsigned)indexPass).demand;
        // this->cumulative_timeWindow[(unsigned)i] = this->cumulative_timeWindow[(unsigned)i-1] + (int)getClientById(this->instance,(unsigned)indexPass).time_window.end; 
        // this->cumulative_distance[(unsigned)i] = this->cumulative_distance[(unsigned)i-1] + 
        //     distance(getClientById(this->instance,(unsigned)indexPass).position,getClientById(this->instance,(unsigned)index).position);
    }

    std::cout << "demandas acumuladas" << std::endl;
    for(int i = 0; i < (int)this->cumulative_demand.size(); i++) {
        std::cout << this->cumulative_demand[(unsigned)i] << " ";
    }
    // std::cout << std::endl;
    // std::cout << "time_windoe acumuladas" << std::endl;
    // for(int i = 0; i < (int)this->cumulative_timeWindow.size(); i++) {
    //     std::cout << this->cumulative_timeWindow[(unsigned)i] << " ";
    // }
    
    std::cout << std::endl;
    
    for(int t = 1; t <= static_cast<int>(this->individual.chromosome.size()); t++) {


        while(this->deque.size() > 1 && calcDistanceRoute(this->deque.front(),t) == -1) {
            std::cout << "erro de janela de tempo:" << std::endl;
            std::cout << "cliente "<< this->deque.front() << " para " << t << std::endl;
            this->deque.pop_front();
        }
        // double pass = this->cumulative_timeWindow[(unsigned)t-1];
        // std::cout << "pass: " << pass << std::endl;
        // double start = 0.0;
        // if(getClientById(this->instance,(unsigned)t).time_window.start > pass+1) { // fazer a verificação do tempo da viagem
        //     start = getClientById(this->instance,(unsigned)t).time_window.start;
        // } else {
        //     start = pass + 1; // um aqui simbolisando o tempo da viagem (adicionar corretamente depois)
        // }
        // this->cumulative_timeWindow[(unsigned)t] = start;
        // std::cout << "this->cumulative_timeWindow[(unsigned)t]: " << this->cumulative_timeWindow[(unsigned)t] << std::endl;
        // if(start > getClientById(this->instance,(unsigned)t).time_window.end) {
        //     std::cout << "caiu time window ruim t: " << t << std::endl;
        //     this->cumulative_timeWindow[(unsigned)t] = 0.0;
        //     this->deque.pop_front();
        //     continue;
        // }
        // if(start <= getClientById(this->instance,(unsigned)t).time_window.end) {
        //     std::cout << "t: " << t << std::endl;
        //     std::cout << "start: " << start << std::endl;
        //     this->cumulative_timeWindow[(unsigned)t] = start;
            double distance = calcDistanceRoute(this->deque.front(),t);
            this->cumulative_distance[(unsigned)t] = this->cumulative_distance[(unsigned)this->deque.front()] + (int)distance;
            // this->cumulative_timeWindow[(unsigned)t] = this->cumulative_timeWindow[(unsigned)t-1] + 
            this->pred[(unsigned)t] = this->deque.front();
    
            if(t < static_cast<int>(this->individual.chromosome.size())) {
                // if(this->deque.size() > 2) {
                //     while(dominates(this->deque.back(),this->deque[this->deque.size()-2],t,5)) {
                //         std::cout << "cliente " << t << " domina " << this->deque.back() << std::endl;
                //         this->deque.pop_back(); // retira do deque o elemento mais antigo
                //     }
                // }
                this->deque.push_back(t);
                while(this->cumulative_demand[(unsigned)t+1] > this->instance.vehicle_capacity + this->cumulative_demand[(unsigned)this->deque.front()+1]) {
                    this->deque.pop_front(); // retira o front do deque, para que na proxima iteracao tenha o melhor candidado no inicio do deque
                }

                // while(true) {
                //     double atual = this->cumulative_timeWindow[(unsigned)(unsigned)t];
                //     double start = this->cumulative_timeWindow[(unsigned)(unsigned)t-1] + getClientById(this->instance,(unsigned)t).time_window.start;
                //     if(getClientById(this->instance,(unsigned)t+1).time_window.start > atual) {
                //         start = getClientById(this->instance,(unsigned)t+1).time_window.start;
                //     } else {
                //         start = atual + 1; // um aqui simbolisando o tempo da viagem (adicionar corretamente depois)
                //     }
                //     if(start < getClientById(this->instance,(unsigned)t+1).time_window.end) {
                        
                //         break;
                //     }
                
                //     this->deque.pop_front();
                // }
    
                // while(this->cumulative_timeWindow[(unsigned)t+1] > getClientById(this->instance,(unsigned)this->deque.front()+1).time_window.end + this->cumulative_timeWindow[(unsigned)this->deque.front()+1]) {
                //     std::cout << "entrou while time window" << std::endl;
                //     std::cout << "this->cumulative_timeWindow[(unsigned)t+1]" << this->cumulative_timeWindow[(unsigned)t+1] << std::endl;
                //     std::cout << "getClientById(this->instance,(unsigned)this->deque.front()+1).time_window.end" << getClientById(this->instance,(unsigned)this->deque.front()+1).time_window.end << std::endl;
                //     std::cout << "this->cumulative_timeWindow[(unsigned)this->deque.front()]" << this->cumulative_timeWindow[(unsigned)this->deque.front()+1] << std::endl;
    
                //     this->deque.pop_front();
                // }
            }
        // }
    }

    for(int i = 1; i < (int)this->pred.size(); i++) {
        std::cout << this->pred[(unsigned)i] << " ";
    }

    std::cout << std::endl;
    
    int end = (int)this->pred.size()-1;
    // std::cout << "end: " << end << std::endl; // ultimo predecessor adicionado
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

    // std::cout << "prev " << prev << std::endl;
    // std::cout << "back " << back << std::endl;
    // std::cout << "t " << t << std::endl;
    // std::cout << "indexBack " << indexBack << std::endl;
    // std::cout << "indexT " << indexT << std::endl;

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
    // j = j > 0 ? j-1 : j;
    // i = i == 0 ? i+1 : i;
    int index = this->individual.chromosome[(unsigned)i];
    double distance = ::distance(this->depot.position, getClientById(this->instance,(unsigned)index).position);
    double fleetTime = distance;
    if(fleetTime < getClientById(this->instance,(unsigned)index).time_window.start) {
        fleetTime = getClientById(this->instance,(unsigned)index).time_window.start;
    }
    std::cout << "i: " << i << std::endl;
    std::cout << "j: " << j << std::endl;
    for(int x = i; x < j-1; x++) { 
        index = this->individual.chromosome[(unsigned)x];
        int indexProx = this->individual.chromosome[(unsigned)x+1];
        std::cout << "index: " << index << std::endl;
        std::cout << "distance antes: " << distance << std::endl;
        std::cout << "fleetTime antes: " << fleetTime << std::endl;
        distance += ::distance(getClientById(this->instance,(unsigned)index).position, getClientById(this->instance,(unsigned)indexProx).position);
        fleetTime += distance;
        
        std::cout << "distance: " << distance << std::endl;
        std::cout << "fleetTime: " << fleetTime << std::endl;


        if(fleetTime < this->instance.clients[(unsigned)indexProx].time_window.start) {
            std::cout << "cliente: " << index << std::endl;
            std::cout << "fleetTime: " << fleetTime << std::endl;
            std::cout << "this->instance.clients[(unsigned)indexProx].time_window.start: " << this->instance.clients[(unsigned)indexProx].time_window.start << std::endl;
            fleetTime = this->instance.clients[(unsigned)index].time_window.start;
        }
        if(fleetTime > this->instance.clients[(unsigned)index].time_window.end) {
            std::cout << "cliente: " << index << std::endl;
            std::cout << "cliente 2: " << index+1 << std::endl;
            std::cout << "fleetTime: " << fleetTime << std::endl;
            std::cout << "this->instance.clients[(unsigned)index].time_window.end: " << this->instance.clients[(unsigned)index].time_window.end << std::endl;
            return -1;
        }
        // std::cout << "dist 2: " << distance << std::endl;
    }
    
    index = this->individual.chromosome[(unsigned)j];
    distance += ::distance(getClientById(this->instance,(unsigned)index).position, this->depot.position);
    fleetTime += distance;
    
    // if(fleetTime < this->instance.clients[(unsigned)index].time_window.start) {
    //     fleetTime = this->instance.clients[(unsigned)index].time_window.start;
    // }

    // if(fleetTime > this->instance.clients[index].time_window.end) return -1;
    // std::cout << "dist 3: " << distance << std::endl;

    return distance;
}