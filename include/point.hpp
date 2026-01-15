#ifndef POINT_HPP
#define POINT_HPP

#include <cmath>

struct Point {
	double x;
	double y;
};

inline auto distance(const Point& a, const Point& b) -> double {
  auto dx = a.x - b.x;
  auto dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

#endif // POINT_HPP