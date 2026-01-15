#include "instance.hpp"
#include "genetic.hpp"
#include "timer.hpp"
#include <iostream>

auto main() -> int {
  auto instance = read_instance("../instances/n20w20l2_1.vrpl");

  auto timer = Timer{};
  auto solution = genetic_algorithm(instance, 100, 500, 0.05);
  auto elapsed_time = timer.elapsed();

  std::cout << "Total distance: " << solution.total_distance << '\n';
  std::cout << "Number of routes: " << solution.routes.size() << '\n';
  std::cout << "Elapsed time (ms): " << elapsed_time << '\n';

  return 0;
}