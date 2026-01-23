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
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
      10.0
    };
    Individual individuo2 = {
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 
        50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 
        70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 
        91, 92, 93, 94, 95, 96, 97, 98, 99},
      10.0
    };

    Split split(instance,individuo);
    split.splitLinear();

    // auto solution = decode_individual(instance, individuo2);
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