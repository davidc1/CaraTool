#ifndef VOXEL_CXX
#define VOXEL_CXX

#include "Voxel.h"

namespace vx {
  
  Voxel::Voxel(int i, int j, int k, double step) {
    
    _i  = i;
    _j  = j;
    _k  = k;
    _hl = step / 2.;
    _e  = 0;
    
  }

}

#endif
