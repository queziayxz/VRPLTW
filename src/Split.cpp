#include <iostream>
#include <stack>

#include "Split.hpp"
// #include "point.hpp"

const double INF = std::numeric_limits<double>::infinity();

Split::Split(Instance instance)
{
    this->instance = instance;
    // this->individual = individual;
    this->depot = instance.depot;

    this->front = 0;
    this->back = 0;
    this->cumulative_distance = std::vector<double>(instance.clients.size()+1);
    this->cumulative_timeWindow = std::vector<double>(instance.clients.size()+1);
    this->cumulative_demand = std::vector<int>(instance.clients.size()+1);
    this->cumulative_capacity_locker = std::vector<int>(instance.lockers.size()+1);
    this->pred = std::vector<int>(instance.clients.size()+1);
}

std::vector<Route> Split::splitLinear(Individual individual)
{
    this->individual = individual;

    this->cumulative_distance[0] = 0;
    this->cumulative_demand[0] = 0;
    this->cumulative_timeWindow[0] = 0.0;
    
    std::vector<DataRoute> bestRoute(this->individual.chromosome.size()+1);

    std::optional<DataRoute> dataRoute;

    // int index = 0;
    int indexPass = 0;

    for(int i = 1; i < (int)this->cumulative_demand.size(); i++) {
        indexPass = this->individual.chromosome[(unsigned)i-1];
        // index = this->individual.chromosome[(unsigned)i];
        this->cumulative_demand[(unsigned)i] = this->cumulative_demand[(unsigned)i-1] + (int)getClientById(this->instance,(unsigned)indexPass).demand;
        // this->cumulative_timeWindow[(unsigned)i] = this->cumulative_timeWindow[(unsigned)i-1] + (int)getClientById(this->instance,(unsigned)indexPass).time_window.end; 
        this->cumulative_distance[(unsigned)i] = INF;
    }

    // std::cout << "demandas acumuladas" << std::endl;
    // for(int i = 0; i < (int)this->cumulative_demand.size(); i++) {
    //     std::cout << this->cumulative_demand[(unsigned)i] << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "distance acumuladas" << std::endl;
    // for(int i = 0; i < (int)this->cumulative_distance.size(); i++) {
    //     std::cout << this->cumulative_distance[(unsigned)i] << " ";
    // }
    
    // std::cout << std::endl;
    
    for(int j = 1; j <= static_cast<int>(this->individual.chromosome.size()); j++) {
        for(int i = j-1; i >= 0; i--) {
            dataRoute = calcDistanceRoute(i,j);
            if(dataRoute.has_value() && !dataRoute->isValid) {
                // std::cout << "erro janela de tempo cliente " << i+1 << " para " << j << std::endl;
                // this->pred[(unsigned)j] = i;
                break;
            }

            // while(this->deque.size() > 1 && distance == -1) {
            //     std::cout << "erro de janela de tempo:" << std::endl;
            //     std::cout << "cliente "<< this->deque.front() << " para " << t << std::endl;
            //     this->deque.pop_front();
            //     distance = calcDistanceRoute(this->deque.front(),t);
            // }

            // std::cout << "i: " << i << std::endl;
            // std::cout << "j: " << j << std::endl;
            // std::cout << "this->cumulative_demand[(unsigned)j]: " << this->cumulative_demand[(unsigned)j] << std::endl;
            // std::cout << "this->cumulative_demand[(unsigned)i]: " << this->cumulative_demand[(unsigned)i] << std::endl;
            // std::cout << "this->instance.vehicle_capacity: " << this->instance.vehicle_capacity << std::endl;

            if(this->cumulative_demand[(unsigned)j] > this->instance.vehicle_capacity + this->cumulative_demand[(unsigned)i]) {
                // std::cout << "excedeu demanda veiculo: " << std::endl;
                break; // passa para a proxima iteracao
            }

            // std::cout << "rota retornada" << std::endl;
            // for(int i = 0; i < (int)dataRoute->route.size(); i++) {
            //     std::cout << dataRoute->route[(unsigned)i] << " ";
            // }
            // std::cout << std::endl;

            double distanceCumulative = this->cumulative_distance[(unsigned)i] + dataRoute->distance;

            // std::cout << "this->cumulative_distance[(unsigned)i]: " << this->cumulative_distance[(unsigned)i] << std::endl;
            // std::cout << "this->cumulative_distance[(unsigned)j]: " << this->cumulative_distance[(unsigned)j] << std::endl;
            // std::cout << "dataRoute->distance: " << dataRoute->distance << std::endl;
            // std::cout << "distanceCumulative: " << distanceCumulative << std::endl;

            if(distanceCumulative < this->cumulative_distance[(unsigned)j]) {
                this->cumulative_distance[(unsigned)j] = distanceCumulative;
                this->pred[(unsigned)j] = i;
                bestRoute[j] = dataRoute.value();
                // std::cout << "rota " << std::endl;
                // std::cout << "size 1: " << bestRoute[j].size() << std::endl;
                // std::cout << "size 2: " << dataRoute->route.size() << std::endl;
                // for(int i = 0; i < bestRoute[j].size(); i++) {
                //     std::cout << bestRoute[j][i] << " ";
                // }
                // std::cout << std::endl;
            }
        }




        // this->cumulative_timeWindow[(unsigned)t] = this->cumulative_timeWindow[(unsigned)t-1] + 
        // this->cumulative_distance[(unsigned)t] = this->cumulative_distance[(unsigned)this->deque.front()] + distance;

        // if(t < static_cast<int>(this->individual.chromosome.size())) {
        //     // if(this->deque.size() > 2) {
        //     //     while(dominates(this->deque.back(),this->deque[this->deque.size()-2],t,5)) {
        //     //         std::cout << "cliente " << t << " domina " << this->deque.back() << std::endl;
        //     //         this->deque.pop_back(); // retira do deque o elemento mais antigo
        //     //     }
        //     // }
        //     this->deque.push_back(t);
        //     while(this->cumulative_demand[(unsigned)t+1] > this->instance.vehicle_capacity + this->cumulative_demand[(unsigned)this->deque.front()+1]) {
        //         this->deque.pop_front(); // retira o front do deque, para que na proxima iteracao tenha o melhor candidado no inicio do deque
        //     }
        //     double distanceNext = calcDistanceRoute(this->deque.front(),t+1);
        //     if(distanceNext < this->cumulative_distance[(unsigned)this->deque.front()]) {
        //         // this->cumulative_distance[(unsigned)this->deque.front()] = distance;
        //         std::cout << "nex encontrou rota melhor" << std::endl;
        //         std::cout << "this->deque.front(): " << this->deque.front() << std::endl;
        //         std::cout << "this->cumulative_distance[(unsigned)this->deque.front()]: " << this->cumulative_distance[(unsigned)this->deque.front()] << std::endl;
        //         std::cout << "distanceNext: " << distanceNext << std::endl;
        //     }

        // }
    }

    // for(int i = 1; i < (int)this->pred.size(); i++) {
    //     std::cout << this->pred[(unsigned)i] << " ";
    // }

    // std::cout << std::endl;
    
    int end = (int)this->pred.size()-1;
    std::vector<Route> routesFinals = std::vector<Route>{};
    // std::cout << "end: " << end << std::endl; // ultimo predecessor adicionado
    int j = 1;
    int x = 0;
    // std::cout << "bestRoute.size(): " << bestRoute.size() << std::endl;
    while(end > 1) {
        int init = (int)this->pred[(unsigned)end];
        // std::cout << "Rota (" << bestRoute[end].distance << "): " << j << " cliente " << init+1 << " -> " << end << std::endl;
        // for(int i = init+1; i <= end; i++) {
        Route newRoute{};
        routesFinals.push_back(newRoute);
        routesFinals[x].customers = std::vector<int>(bestRoute[end].route.size());
        routesFinals[x].assigned_lockers = std::vector<int>(bestRoute[end].route.size());
        routesFinals[x].total_distance = bestRoute[end].distance;
        routesFinals[x].load = bestRoute[end].load;
        for(int i = 0; i < bestRoute[end].route.size(); i++) {
            // std::cout << this->individual.chromosome[i-1] << " ";
            routesFinals[x].customers[i] = bestRoute[end].customers[i];
            // std::cout << "cliente visitado: " << routesFinals[x].customers[i] << std::endl;
            if(bestRoute[end].route[i] > 0) {
                // std::cout << "size best: " << bestRoute[end].route.size() << std::endl;
                // std::cout << "size customers: " << routesFinals[x].customers.size() << std::endl;
                // std::cout << "eh cliente" << std::endl;
                routesFinals[x].assigned_lockers[i] = -1;
            } else {
                // std::cout << "eh locker" << std::endl;
                routesFinals[x].assigned_lockers[i] = abs(bestRoute[end].route[i]);
            }
            // std::cout << bestRoute[end].route[i] << " ";
        }
        end = init;
        j++;
        x++;
        // std::cout << std::endl;
    }
    // std::cout << "f(0,2): " << calcDistanceRoute(0,2) << std::endl; // 0,1

    return routesFinals;

}

// bool Split::dominates(int back, int prev, int t, int capacity)
// {
//     if(this->deque.size() < 2) return false; // nao possui elementos suficientes para verificar a dominacao

//     int indexBack = indexDominates(prev,back,capacity);
//     int indexT = indexDominates(back,t,capacity);

//     // std::cout << "prev " << prev << std::endl;
//     // std::cout << "back " << back << std::endl;
//     // std::cout << "t " << t << std::endl;
//     // std::cout << "indexBack " << indexBack << std::endl;
//     // std::cout << "indexT " << indexT << std::endl;

//     if(indexT <= indexBack) return true;

//     return false;
// }

// int Split::indexDominates(int a, int b, int capacity)
// {
//     int low = b;
//     int hight = std::min((int)this->individual.chromosome.size()-1, b+capacity);
//     int mid = 0;
//     int best = hight + 1; // soma mais um no limite para caso retorne esse valor, ele ser menor na comparação do dominates

//     while(low < hight) {
//         mid = (low+hight)/2; // calcula ponto medio do intervalo
//         double costA = this->cumulative_distance[(unsigned)a] + calcDistanceRoute(a,mid);
//         double costB = this->cumulative_distance[(unsigned)b] + calcDistanceRoute(b,mid);

//         if(costB < costA) { // B ja eh melhor que A nesse intervalor
//             best = mid;
//             hight = mid - 1;
//         } else { // B ainda nao eh melhor que A no intervalo
//             low = mid + 1; 
//         }
//     }

//     return best;
// }

std::optional<DataRoute> Split::calcDistanceRoute(int i, int j)
{
    // j = j > 0 ? j-1 : j;
    // i = i == 0 ? i+1 : i;
    std::optional<DataRoute> dataRoute = DataRoute{};
    std::optional<Locker> locker;
    Point prev = this->depot.position; // rota sempre sai do deposito
    double cumulative_cost = 0.0;

    int locationChosen = 0; // 0 deposito / id positivo cliente / id negativo locker 

    int index = this->individual.chromosome[(unsigned)i];
    // Client initClient = getClientById(this->instance,(unsigned)index);
    
    // double distance = ::distance(this->depot.position, initClient.position);
    double distance = 0.0;
    double load = 0.0;
    double fleetTime = distance;
    // if(fleetTime < initClient.time_window.start) {
    //     fleetTime = initClient.time_window.start;
    // }

    bool useClient = true;
    bool dangerFleetTime = false;
    // bool useLocker = false;

    // stackRoute.push(index); //posso usar valores negativos para os lockers o negativo do seu id

    // std::cout << "i: " << i << std::endl;
    // std::cout << "j: " << j << std::endl;
    for(int x = i; x <= j-1; x++) { 
        // std::cout << "x: " << x << std::endl;
        index = this->individual.chromosome[(unsigned)x];
        // std::cout << "index: " << index << std::endl;
        // std::cout << "distance atual: " << distance << std::endl;
        int indexProx = this->individual.chromosome[(unsigned)x+1];
        // std::cout << "indexProx: " << indexProx << std::endl;

        Client clientA = getClientById(this->instance,(unsigned)index);
        Client clientB = getClientById(this->instance,(unsigned)indexProx);
        // double distanceTemporary = distance + ::distance(clientA.position, getClientById(this->instance,(unsigned)indexProx).position);
        double distanceTemporary = distance + ::distance(clientA.position, prev) + ::distance(clientA.position, clientB.position);
        // if (distance == 0) distanceTemporary += ::distance(clientA.position, prev);

        if(locationChosen >= 0 && x < j-1) { // cliente ou deposito ultmo visitado
            // std::cout << "cliente anterior" << std::endl;
            locker = getLockerCommon(clientA,clientB,dataRoute->lockers); // verifica se tem locker em comum
            // locker = getBestLockerDistance(clientA,dataRoute->lockers);
            // std::cout << "cliente anterior" << std::endl;
            if(locker.has_value()) {
                // std::cout << "tem locker comum" << std::endl;
                cumulative_cost = 0;
                cumulative_cost += ::distance(clientA.position,locker->position);
                cumulative_cost += ::distance(clientB.position,locker->position);
                if(verifyBestLocker(locker.value(),prev,cumulative_cost,distanceTemporary,dataRoute->lockers)) { // se for true entao eh melhor adicionar no locker
                    // std::cout << "locker melhor: " << x << std::endl;
                    locationChosen = -(locker->id);
                    dataRoute->route.push_back((int)locationChosen);
                    dataRoute->route.push_back((int)locationChosen);
                    dataRoute->lockers.insert(locker->id);

                    dataRoute->customers.push_back((int)clientA.id);
                    dataRoute->customers.push_back((int)clientB.id);
                    load += clientA.demand;
                    load += clientB.demand;
                    useClient = false;

                    dangerFleetTime = false;
                    dataRoute->isValid = true;
                    distance += ::distance(prev,locker->position);
                    x++; // soma para nao visitar o cliente b novamente
                    // std::cout << "locker melhor: " << x << std::endl; 
                } else {
                    // distance = distanceTemporary;
                    useClient = true;
                }
            } else {
                // std::cout << "nao tem locker comum" << std::endl;
                // prev = clientA.position;
                useClient = true;
            }
        } else if(locationChosen < 0) {
            // std::cout << "locker anterior: " << clientA.id << std::endl;
            // std::cout << "locker anterior: " << x << std::endl;
            // std::cout << "clientB.delivery_locations.size(): "<< clientB.delivery_locations.size() << std::endl;
            // if(locker.has_value()) std::cout << "locker existe" << std::endl;
            if(clientA.delivery_locations.size() > 0 && verifyLocationLocker(locker.value(),clientA,dataRoute->lockers)) {
                // std::cout << "pode adicionar locker:  " << std::endl;
                cumulative_cost += ::distance(clientA.position,locker.value().position);
                if(verifyBestLocker(locker.value(),prev,cumulative_cost,distanceTemporary,dataRoute->lockers)) { // se for true entao eh melhor adicionar no locker
                    // std::cout << "locker melhor: " << x << std::endl;
                    locationChosen = -(locker.value().id);
                    dataRoute->route.push_back((int)locationChosen);

                    dataRoute->customers.push_back((int)clientA.id);
                    load += clientA.demand;
                    // dataRoute->lockers.insert(locker->id);

                    // std::cout << "adicionou cliente tambem " << clientA.id << " no locker " << locker->id << std::endl;

                    dangerFleetTime = false;
                    dataRoute->isValid = true;
                    useClient = false;
                    // std::cout << "adicionou tambem cliente " << clientB.id << " no locker " << locker.value().id << std::endl;
                    // std::cout << "locker melhor: " << x << std::endl;
                    // distance += ::distance(prev,locker.value().position);
                    // std::cout << "locker melhor: " << x << std::endl;
                    // continue;
                }
            } else {
                // std::cout << "nao pode adicionar locker" << std::endl;
                // prev = locker.value().position; // cliente A já estaria no locker, mas o B nao pode entrar, aqui na verdade o prev previsa ser um locker
                useClient = true;
            }
        }

        // if(dangerFleetTime) {
        //     std::cout << "continua com erro de janela de tempo dentro" << std::endl;

        //     if(clientA.delivery_locations.size() == 0) return dataRoute;
            
        //     std::optional<Locker> lockerAdd = getBestLockerDistance(clientA,dataRoute->lockers);
        //     if(!lockerAdd.has_value()) return dataRoute;

        //     // std::cout << "resolvendo problema: " << x << std::endl;
            
        //     locationChosen = -(lockerAdd->id);
        //     dataRoute->route.push_back((int)locationChosen);
        //     dataRoute->lockers.insert(locker->id);
        //     prev = lockerAdd->position;
            
        //     dangerFleetTime = false;
        //     dataRoute->isValid = true;
        //     useClient = false;
            
        //     distance += ::distance(prev,lockerAdd->position);
        //     locker = lockerAdd;
            
        //     std::cout << "adicionou cliente erro janela " << clientA.id << " no locker " << lockerAdd->id << std::endl;
        //     useClient = false;
        // };

        if(useClient) {
            // std::cout << "cliente melhor" << std::endl;
            dataRoute->route.push_back((int)clientA.id);
            dataRoute->customers.push_back((int)clientA.id);
            locationChosen = clientA.id;
            load += clientA.demand;
            distance += ::distance(prev,clientA.position);
            // std::cout << "distance depois de adicionar cliente " << clientA.id << ": " << distance << std::endl;
            // std::cout << "size: " << dataRoute->route.size() << std::endl;
            // distance = distanceTemporary;
        }
    
        fleetTime += distance;

        if(fleetTime < clientA.time_window.start && useClient) {
            // std::cout << "caminhao chegou antes: " << fleetTime << std::endl;
            fleetTime = clientA.time_window.start;
            // std::cout << "caminhao chegou antes (valor depois): " << fleetTime << std::endl;
        }
        
        if(fleetTime > clientA.time_window.end && useClient) {
            // std::cout << "erro janela de tempo" << std::endl;
            // std::cout << "clientA.time_window.end: " << clientA.time_window.end << std::endl;
            // std::cout << "cliente: " << clientA.id << std::endl;

            dangerFleetTime = true;
            dataRoute->isValid = false;

            // std::cout << "continua com erro de janela de tempo dentro" << std::endl;

            if(clientA.delivery_locations.size() == 0){
                // std::cout << "cliente " << clientB.id << " nao possui nenhum locker" << std::endl;
                return dataRoute;
            } 
            
            std::optional<Locker> lockerAdd = getBestLockerDistance(clientA,dataRoute->lockers);
            if(!lockerAdd.has_value()) {
                return dataRoute;
            } 

            // std::cout << "resolvendo problema: " << x << std::endl;
            
            locationChosen = -(lockerAdd->id);
            dataRoute->route.pop_back(); // retira o cliente adicionado antes
            // load -= clientA.demand;
            dataRoute->route.push_back((int)locationChosen); // adiciona o locker no lugar
            dataRoute->lockers.insert(locker->id);
           
            dangerFleetTime = false;
            dataRoute->isValid = true;
            useClient = false;
            
            // std::cout << "prev.x: " << prev.x << std::endl; 
            // std::cout << "prev.y: " << prev.y << std::endl; 
            distance = distance - ::distance(prev,clientA.position); // retira a soma da distância anterior somada
            // std::cout << "distancia depois de retirar o cliente " << clientA.id << " : " << distance << std::endl; 
            
            distance += ::distance(prev,lockerAdd->position);
            // std::cout << "distancia depois de adicionar o locker " << lockerAdd->id << " : " << distance << std::endl; 
            locker = lockerAdd;
            prev = lockerAdd->position;

            // x++; // pula para o cliente b nao aparecer na proxima iteracao
            
            // std::cout << "adicionou cliente erro janela " << clientA.id << " no locker " << lockerAdd->id << std::endl;
            continue;
        }

        prev = clientA.position; // caso o cliente tenha sido escolhido
        
    }

    // index = this->individual.chromosome[(unsigned)j-1];

    // if(dangerFleetTime) {
    //     std::cout << "continua com erro de janela de tempo" << std::endl;
    //     dataRoute->isValid = false;

    //     Client clientError = getClientById(this->instance,(unsigned)index);
    //     if(clientError.delivery_locations.size() == 0) return dataRoute;
        
    //     std::optional<Locker> lockerAdd = getBestLockerDistance(clientError,dataRoute->lockers);
    //     if(!lockerAdd.has_value()) return dataRoute;

    //     dataRoute->route.push_back(-lockerAdd->id);
    //     dataRoute->lockers.insert(locker->id);
        
    //     dataRoute->isValid = true;

    //     // std::cout << "resolvendo problema: " << std::endl;
    //     distance += ::distance(prev,lockerAdd->position);
    //     // std::cout << "adicionou cliente " << clientError.id << " no locker " << lockerAdd.id << std::endl;
    // }

    if(locationChosen >= 0) {
        // std::cout << "cliente foi o ultimo viisitado na rota: " << index << std::endl;
        distance += ::distance(getClientById(this->instance,(unsigned)index).position, this->depot.position);
    } else { // siginifica que o ultimo visitado foi um locker
        // std::cout << "locker foi o ultimo viisitado na rota: " << locker->id << std::endl;
        distance += ::distance(locker->position, this->depot.position);

    }
    fleetTime += distance;
    
    if(dataRoute.has_value()) {
        // std::cout << "entrou true" << std::endl;
        // std::cout << "distancia final (calcDistance): " << distance << std::endl;
        dataRoute->distance = distance;
        dataRoute->time = fleetTime;
        dataRoute->load = load;
        // dataRoute->route.push_back(index);
    }
    
    return dataRoute;
}

bool Split::verifyBestLocker(Locker&locker, Point&prev, double cumulative_cost, double cumulativeDistance, std::unordered_set<int>&lockersUsed)
{
    double distancePrevLocker = 0.0;

    distancePrevLocker = distance(prev,locker.position);

    // std::cout << "distancePrevLocker: " << distancePrevLocker << std::endl;
    // std::cout << "cumulative_cost: " << cumulative_cost << std::endl;
    // std::cout << "cumulativeDistance: " << cumulativeDistance << std::endl;

    if(cumulativeDistance > distancePrevLocker+cumulative_cost) return true; // significa que vale mais a pena adicionar no locker mesmo pagando um preco

    return false;
}

bool Split::verifyLocationLocker(Locker&locker, Client&clientB, std::unordered_set<int>&lockersUsed)
{
    for(Locker&lock : clientB.delivery_locations) {
        // if(lock.id == locker.id) return true;
        if(lock.id == locker.id && lockersUsed.count((int)lock.id) == 0) return true;
    }

    return false;
}

std::optional<Locker> Split::getBestLockerDistance(Client&client, std::unordered_set<int>&lockersUsed)
{
    double distance = INF;
    std::optional<Locker> locker{};

    for(Locker&lock : client.delivery_locations) {
        if(::distance(lock.position,client.position) < distance && lockersUsed.count((int)lock.id) == 0) {
        // if(::distance(lock.position,client.position) < distance) {
            locker = lock;
            distance = ::distance(lock.position,client.position);
        }
    }

    return locker;
}

// retorna o primeiro locker em comum
// nao verifica a distancia
std::optional<Locker> Split::getLockerCommon(Client&clientA, Client&clientB, std::unordered_set<int>&lockersUsed) 
{
    std::optional<Locker> locker{};
    
    // std::cout << "size lockers A: " << clientA.delivery_locations.size() << std::endl;
    // std::cout << "size lockers B: " << clientB.delivery_locations.size() << std::endl;

    for(Locker lockA : clientA.delivery_locations) {
        for(Locker lockB : clientB.delivery_locations) {
            // std::cout << "lockA: " << lockA.id << std::endl;
            // std::cout << "lockB: " << lockB.id << std::endl;
            // if(lockA.id == lockB.id) return lockB;
            if(lockA.id == lockB.id && lockersUsed.count((int)lockB.id) == 0) return lockB;
        }
    }

    return locker;
}