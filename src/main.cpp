#include "instance.hpp"
#include "genetic.hpp"
#include "Split.hpp"
#include "timer.hpp"
#include <iostream>

auto main() -> int {

  try {
    auto instance = read_instance("../instances/n20w20l2_1.vrpl");
  
    // auto timer = Timer{};
    // auto solution = genetic_algorithm(instance, 100, 500, 0.05);
    Individual individuo = {
      {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20},
      10.0
    };

    Split split(instance,individuo);
    split.splitLinear();

    // auto solution = decode_individual(instance, individuo);
    // // auto elapsed_time = timer.elapsed();
  
    // // std::cout << "Total da distancia: " << solution.total_distance << '\n';
    // std::cout << "Numero de rotas: " << solution.size() << '\n';
    // // std::cout << "Tempo total (ms): " << elapsed_time << '\n';

    // for(auto i = 0u; i < solution.size(); i++) {
    //   std::cout << "Rota (" << solution[i].total_distance << ")(" << solution[i].load << "):";
    //   for(auto j = 0u; j < solution[i].customers.size(); j++) {
    //     if(solution[i].assigned_lockers[j] == -1) {
    //       std::cout << "Cliente " << solution[i].customers[j] << ", ";
    //     } else {
    //       std::cout << "Locker " << solution[i].assigned_lockers[j] << ", ";
    //     }
    //   }
    //   std::cout << std::endl;
    // }

  } catch(std::runtime_error&e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}