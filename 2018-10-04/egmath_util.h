#ifndef _GMATH_UTIL_H
#define _GMATH_UTIL_H

#include <vector>

class GPoint3;
class GVector2;

int get_gcd(std::vector<int> &NumList);
void find_convexhull_2d(const std::vector<GPoint3> &InPts, std::vector<GPoint3> &OutPts);
bool pt_in_polygon(const std::vector<GPoint3> &Pts, GPoint3 const &p);
bool pt_in_triangle(GVector2 p, GVector2 p1, GVector2 p2, GVector2 p3);

#endif // _GMATH_UTIL_H