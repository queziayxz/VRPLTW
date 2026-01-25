#include "instance.hpp"
#include "genetic.hpp"
#include "Split.hpp"
#include "LocalSearch.hpp"
#include "timer.hpp"
#include <iostream>
#include <array>
#include <fstream>
#include <string>

auto main() -> int {
  constexpr auto instances = std::array{
    "n20w100l2_1",
    "n20w100l2_2",
    "n20w100l2_3",
    "n20w100l2_4",
    "n20w100l2_5",
    "n20w20l2_1",
    "n20w20l2_2",
    "n20w20l2_3",
    "n20w20l2_4",
    "n20w20l2_5",
    "n40w100l4_1",
    "n40w100l4_2",
    "n40w100l4_3",
    "n40w100l4_4",
    "n40w100l4_5",
    "n40w20l4_1",
    "n40w20l4_2",
    "n40w20l4_3",
    "n40w20l4_4",
    "n40w20l4_5",
    "n60w100l6_1",
    "n60w100l6_2",
    "n60w100l6_3",
    "n60w100l6_4",
    "n60w100l6_5",
    "n60w20l6_1",
    "n60w20l6_2",
    "n60w20l6_3",
    "n60w20l6_4",
    "n60w20l6_5",
    "n100w100l10_1",
    "n100w100l10_2",
    "n100w100l10_3",
    "n100w100l10_4",
    "n100w100l10_5",
    "n100w20l10_1",
    "n100w20l10_2",
    "n100w20l10_3",
    "n100w20l10_5",
  };

  constexpr auto instance_prefix = std::string{"../instances/"};
  constexpr auto instance_suffix = ".vrpl";

  auto results = std::ofstream{"../results.txt"};

  results << "Instance BestCost BestTime(s) AvgCost AvgTime(s)\n";

  for (auto instance_id = 1; unsigned(instance_id) <= instances.size(); ++instance_id) {
    const auto* instance_name = instances[unsigned(instance_id) - 1];

    std::cout << "> Running instance: " << instance_name << " (" << instance_id << '/' << instances.size() << ")\n";

    auto total_time = 0.0;
    auto total_cost = 0.0;
    auto best_time = 0.0;
    auto best_cost = std::numeric_limits<double>::max();

    constexpr auto runs = 5;

    auto instance = read_instance(instance_prefix + instance_name + instance_suffix);

    for (auto i = 0; i < runs; ++i) {
      auto timer = Timer{};
      auto solution = genetic_algorithm(instance, 60, 100, 0.05);
      auto elapsed_time = timer.elapsed() / 1000.0;

      total_time += elapsed_time;
      total_cost += solution.total_distance;
      
      if (solution.total_distance < best_cost) {
        best_cost = solution.total_distance;
        best_time = elapsed_time;
      }
    }

    results << instance_name << " "
            << best_cost << " "
            << best_time << " "
            << (total_cost / runs) << " "
            << (total_time / runs) << '\n';
  }

  //   for(auto i = 0u; i < solution.routes.size(); i++) {
  //     std::cout << "Rota (" << solution.routes[i].total_distance << ")(" << solution.routes[i].load << "): ";
  //     for(auto j = 0u; j < solution.routes[i].customers.size(); j++) {
  //       if(solution.routes[i].assigned_lockers[j] == -1) {
  //         std::cout << "C" << solution.routes[i].customers[j] << " - ";
  //         continue;
  //       }
  //       int locker = solution.routes[i].assigned_lockers[j];
  //       std::cout << "L" << locker;
  //       std::cout << "(";
  //       while(solution.routes[i].assigned_lockers[j] == locker) {
  //         std::cout << solution.routes[i].customers[j] << ",";
  //         j++;
  //       }
  //       std::cout << ") - ";
  //       j--;
  //     }
  //     std::cout << std::endl;
  //   }

  return 0;
}