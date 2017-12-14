/**
 * \file VoxelizeTracks.h
 *
 * \ingroup G4Voxel
 * 
 * \brief Class def header for a class VoxelizeTracks
 *
 * @author david
 */

/** \addtogroup G4Voxel

    @{*/
#ifndef VOXELIZETRACKS_H
#define VOXELIZETRACKS_H

#include <iostream>
#include <map>

#include "GeoAlgo/GeoAlgo.h"

#include "Voxel.h"
#include "ID.h"

#include "TTree.h"
#include "TFile.h"

/**
   \class VoxelizeTracks
   User defined class VoxelizeTracks ... these comments are used to generate
   doxygen documentation!
 */

namespace vx {
  
  class VoxelizeTracks{
    
  public:
    
    /// Default constructor
    VoxelizeTracks(){};

    VoxelizeTracks(double step);

    VoxelizeTracks(double step, double dmax);
    
    /// Default destructor
    ~VoxelizeTracks(){}
    
    void AddG4Step(const double& x0, const double& y0, const double& z0,
		   const double& x1, const double& y1, const double& z1,
		   const double& e);

    void setVerbose(bool on) { _verbose = on; }

    void getSummary() { std::cout << "energy added : " << _etot << "\t energy voxelized : " << _evox << std::endl; }

    // apply energy threshold
    double applyThreshold(const double& emin);

    // cluster based on angle and radius
    double Cluster(const double& angle, const double& radius);

    // get dEdx @ shower trunk
    double dEdx(const double& dist);

    void WriteTree();

    void Clear();
    
    // set start point
    void SetStart(const double& x, const double& y, const double& z);

    double TotalEnergy() { return _etot; }
    
  private:
    
    // map linking (i,j,k) to index  in voxel vector
    std::map< ID, Voxel > _voxelMap;
    
    // voxel step size
    double _step;

    // boolean for verbosity
    bool _verbose;
    
    /// get voxel associated to specific coordinates
    Voxel& getVoxel(const double& x, const double& y, const double& z);
    /// get voxel associated to a specific index triplet
    Voxel& getVoxel(const int& i,    const int& j,    const int& k   );
    /// get voxel associated to a specific index
    Voxel& getVoxel(const ID& id);
    
    /** get list of indermediate voxel IDs between two voxels
	assume that the voxels must share at least one edge
	returns vector of IDs which may possibly connect the two
    */
    std::vector<ID> getIntermediateVoxels(const ID& id1, const ID& id2);
    
    
    /// get AABox given a voxel
    ::geoalgo::AABox_t getAAVoxel(const Voxel& thisvoxel);
    
    /// geoalgo utility
    ::geoalgo::GeoAlgo _geoAlgo;

    // keep track of total energy added
    double _etot;
    // keep track of total energy voxelized
    double _evox;
    
    // keep track of 1st point
    bool _first;
    double _xstart, _ystart, _zstart;
    // max distance to voxelize for
    double _dmaxsq;

    TTree *_voxel_tree;
    double _edep;
    double _dx;
    int    _evt;
    
  };

}

#endif
/** @} */ // end of doxygen group 

