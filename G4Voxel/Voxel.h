/**
 * \file Voxel.h
 *
 * \ingroup G4Voxel
 * 
 * \brief Class def header for a class Voxel
 *
 * @author david caratelli
 */

/** \addtogroup G4Voxel

    @{*/
#ifndef VOXEL_H
#define VOXEL_H

#include <iostream>

/**
   \class Voxel
   User defined class Voxel ... these comments are used to generate
   doxygen documentation!
 */

namespace vx {
  
  class Voxel{
    
  public:
    
    /// Default constructor
    Voxel(){}
    
    Voxel(int i, int j, int k, double step);
    
    /// Default destructor
    ~Voxel(){}
    
    void AddEnergy(double e) { _e += e; }
    
    double x() const { return (2 * _i + 1) * _hl; }
    double y() const { return (2 * _j + 1) * _hl; }
    double z() const { return (2 * _k + 1) * _hl; }
    double HalfLength() const { return _hl; }
    double Energy()     const { return _e; }
    
  private:
    
    int _i, _j, _k; // indices of this voxel
    double _hl;     // half-lefth of voxel
    double _e;      // energy in voxel
    
  };

}
  
#endif
/** @} */ // end of doxygen group 

