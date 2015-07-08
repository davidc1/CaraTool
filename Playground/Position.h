#ifndef POSITION_H
#define POSITION_H

#include <cmath>
#include <vector>
#include <iostream>

namespace radius{

  class Point : public std::vector<double> {
    
  public:

    Point() {};

  Point(const std::vector<double> &v) : std::vector<double>(v)
      {}
    
    virtual ~Point(){};

    bool operator< (const Point& p1)
    {
      for (size_t i=0; i < 3; i++){
	if ((*this)[i] < p1[i]) return true;
	if ((*this)[i] > p1[i]) return false;
      }
      return false;
    }

  private:

  };
}

#endif
